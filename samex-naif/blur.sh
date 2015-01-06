# Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
# Licensed AS IS and WITHOUT WARRANTY under the Apache License,
# Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>


#
# Argument parsing
#

# Whether to compile for profiling.
profile=0

# Whether to compile with optimiaztions on.
optimize=1

while [[ $1 != '' ]]; do
    opt="$1"
    if [[ ${opt} =~ ^--name=(.*) ]]; then
        PROJECT_NAME="${BASH_REMATCH[1]}"
    elif [[ ${opt} == '--no-optimize' ]]; then
        optimize=0
    elif [[ ${opt} == '--profile' ]]; then
        profile=1
    else
        echo "Unknown option: ${opt}" 1>&2
        exit 1
    fi
    shift
done
unset opt


#
# Main script
#

OUT="${BASE_DIR}/out"
FINAL="${OUT}/final"

binName='samex' # Name of executable in the `lib` directory.

INTERMED="${OUT}/intermed/${PROJECT_NAME}"
FINAL_INCLUDE="${FINAL}/include/${PROJECT_NAME}"
FINAL_LIB="${FINAL}/lib/${PROJECT_NAME}"
FINAL_EXE="${FINAL_LIB}/${binName}"

# Set `$CC` to `"cc"` if it's not already set.
if [[ ${CC} == '' ]]; then
    CC='cc'
fi

# Figure out what OS we have, and set up build commands accordingly. See the
# build code in <https://github.com/danfuzz/dl-example> for details about all
# the compiler options.

if [[ ${OSTYPE} == '' ]]; then
    OSTYPE="$(uname)"
fi

case "${OSTYPE}" in
    (linux* | Linux*)
        WHAT_OS='linux'
    ;;
    (darwin* | Darwin* | *bsd* | *BSD*)
        WHAT_OS='bsd'
    ;;
    (*)
        echo 1>&2 "Sorry: Unknown OS type: ${OSTYPE}"
        exit 1
    ;;
esac

CC_PREFIX=("${CC}")

if (( profile )); then
    CC_PREFIX+=(-pg)
fi

if (( optimize )); then
    CC_PREFIX+=(-O3)
fi

COMPILE_C=("${CC_PREFIX[@]}" -std=c99 -g -c -I"${PROJECT_DIR}/include")

LINK_BIN=("${CC_PREFIX[@]}" -g)
LINK_BIN_SUFFIX=()

if [[ ${WHAT_OS} == 'linux' ]]; then
    LINK_BIN+=(-rdynamic)
    LINK_BIN_SUFFIX+=(-ldl)
fi

# Rules to copy each library source file to the final lib directory.

LIB_SOURCE_BASE="${BASE_DIR}/samlib-naif"
LIB_FILES=($(cd "${LIB_SOURCE_BASE}"; find . -name '*.sam*'))

rule copy \
    --id=build-lib \
    --in-dir="${LIB_SOURCE_BASE}" \
    --out-dir="${FINAL_LIB}/corelib" \
    -- "${LIB_FILES[@]}"

# Rules to copy each include file to the final include directory.

INCLUDE_SOURCE_BASE="${PROJECT_DIR}/include"
INCLUDE_FILES=($(cd "${INCLUDE_SOURCE_BASE}"; find . -name '*.h'))

rule copy \
    --id=build-include \
    --in-dir="${INCLUDE_SOURCE_BASE}" \
    --out-dir="${FINAL_INCLUDE}" \
    -- "${INCLUDE_FILES[@]}"

# Rules to compile each C source file.

C_SOURCES=($(find . -name '*.c'))
C_OBJECTS=()

for file in "${C_SOURCES[@]}"; do
    [[ ${file} =~ ^\./(.*)/([^/]*)\.c$ ]] || exit 1
    dir="${BASH_REMATCH[1]}"
    baseName="${BASH_REMATCH[2]}"
    outDir="${INTERMED}/${dir}"
    outFile="${outDir}/${baseName}.o"
    inFile="${PROJECT_DIR}/${file}"

    C_OBJECTS+=("${outFile}")
    rule mkdir -- "${outDir}"

    rule body \
        --id=build-c \
        --req="${inFile}" \
        --req="${outDir}" \
        --target="${outFile}" \
        --msg="Compile: ${file#./}" \
        --cmd="$(quote "${COMPILE_C[@]}" -o "${outFile}" "${inFile}")"
done

# Rules to link the executable

rule mkdir -- "${FINAL_LIB}"

rule body \
    --id=link-bin \
    --target="${FINAL_EXE}" \
    --req="${FINAL_LIB}" \
    "${C_OBJECTS[@]/#/--req=}" \
    --msg="Link: ${FINAL_EXE}" \
    --cmd="$(quote "${LINK_BIN[@]}" -o "${FINAL_EXE}" "${C_OBJECTS[@]}" \
        ${LINK_BIN_SUFFIX[@]})"

# Rule to clean stuff

rule rm \
    --id=clean \
    -- "${FINAL_EXE}" "${FINAL_INCLUDE}" "${FINAL_LIB}" "${INTERMED}"

# Default build rules

rule body \
    --id=external-reqs \
    --build-in-dir="../samex"

rule body \
    --id=build \
    --req-id=external-reqs \
    --req-id=link-bin \
    --req-id=build-lib \
    --req-id=build-include
