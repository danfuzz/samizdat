#!/bin/bash
#
# Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
# Licensed AS IS and WITHOUT WARRANTY under the Apache License,
# Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>


#
# General setup
#

# Set `progName` to the program name, `progDir` to its directory, and `baseDir`
# to `progDir`'s directory. Follows symlinks.
function initProg {
    local newp prog="$0"

    while newp="$(readlink "${prog}")"; do
        [[ ${newp} =~ ^/ ]] && prog="${newp}" || prog="${prog%/*}/${newp}"
    done

    progName="${prog##*/}"
    progDir="$(cd "${prog%/*}"; /bin/pwd -P)"
    baseDir="$(cd "${progDir}/.."; /bin/pwd -P)"
}

initProg
. "${baseDir}/blur/blur-utils.sh"


#
# Argument parsing
#

# Whether to compile for profiling.
profile='no'

# Whether to compile with optimiaztions on.
optimize='no'

# Name of executable to produce.
name='samex-naif'

while [[ $1 != '' ]]; do
    opt="$1"
    if [[ ${opt} == '--' ]]; then
        shift
        break
    elif [[ ${opt} == '--help' ]]; then
        echo "${progName} [--optimize] [--profile] [--name=<name>]"
        exit
    elif [[ ${opt} =~ ^--name=(.*) ]]; then
        name="${BASH_REMATCH[1]}"
    elif [[ ${opt} == '--optimize' ]]; then
        optimize='yes'
    elif [[ ${opt} == '--profile' ]]; then
        profile='yes'
    elif [[ ${opt} =~ ^- ]]; then
        echo "Unknown option: ${opt}" 1>&2
        exit 1
    else
        break
    fi
    shift
done
unset opt


#
# Main script
#

projectName="$(basename "${progDir}")"
binName='samex' # Name of executable in the `lib` directory.

OUT="${OUT:-${baseDir}/out}"
INTERMED="${OUT}/intermed/${projectName}"
FINAL="${OUT}/final"
FINAL_INCLUDE="${FINAL}/include/${name}"
FINAL_LIB="${FINAL}/lib/${name}"
FINAL_EXE="${FINAL_LIB}/${binName}"

if [[ ${profile} == 'yes' ]]; then
    CC=(cc -pg)
else
    CC=(cc)
fi

if [[ ${optimize} == 'yes' ]]; then
    CC=(${CC[@]} -O3)
fi

COMPILE_C=("${CC[@]}" -g -c -I"${progDir}/include")
LINK_BIN=("${CC[@]}" -g)

# Rules to copy each library source file to the final lib directory.

LIB_SOURCE_BASE="${progDir}/../samlib-naif"
LIB_FILES=($(cd "${LIB_SOURCE_BASE}"; find . -name '*.sam*'))

rule copy \
    --id=build-lib \
    --from-dir="${LIB_SOURCE_BASE}" \
    --to-dir="${FINAL_LIB}" \
    "${LIB_FILES[@]}"

# Rules to copy each include file to the final include directory.

INCLUDE_SOURCE_BASE="${progDir}/include"
INCLUDE_FILES=($(cd "${INCLUDE_SOURCE_BASE}"; find . -name '*.h'))

rule copy \
    --id=build-include \
    --from-dir="${INCLUDE_SOURCE_BASE}" \
    --to-dir="${FINAL_INCLUDE}" \
    "${INCLUDE_FILES[@]}"

# Rules to compile each C source file.

C_SOURCES=($(cd "${progDir}"; find . -name '*.c'))
C_OBJECTS=()

for file in "${C_SOURCES[@]}"; do
    [[ ${file} =~ ^\./(.*)/([^/]*)\.c$ ]] || exit 1
    dir="${BASH_REMATCH[1]}"
    baseName="${BASH_REMATCH[2]}"
    outDir="${INTERMED}/${dir}"
    outFile="${outDir}/${baseName}.o"
    inFile="${progDir}/${file}"

    C_OBJECTS+=("${outFile}")
    rule mkdir "${outDir}"

    printf 'start\n'
    printf '  id build-c\n'
    printf '  req %q\n' "${inFile}"
    printf '  req %q\n' "${outDir}"
    printf '  target %q\n' "${outFile}"
    printf '  msg Compile: %q\n' "${file#./}"
    printf '  cmd '
        printf '%q ' "${COMPILE_C[@]}" -o "${outFile}" "${inFile}"
        printf '\n'
    printf 'end\n'
done

# Rules to link the executable

rule mkdir "${FINAL_LIB}"

printf 'start\n'
printf '  id link-bin\n'
printf '  target %q\n' "${FINAL_EXE}"
printf '  req %q\n' "${FINAL_LIB}"
printf '  req %q\n' "${C_OBJECTS[@]}"
printf '  msg Link: %q\n' "${FINAL_EXE}"
printf '  cmd '
    printf '%q ' "${LINK_BIN[@]}" -o "${FINAL_EXE}" "${C_OBJECTS[@]}"
    printf '\n'
printf 'end\n'

# Rule to clean stuff

printf 'start\n'
printf '  id clean\n'
printf '  cmd rm -rf %q\n' "${FINAL_EXE}"
printf '  cmd rm -rf %q\n' "${FINAL_INCLUDE}"
printf '  cmd rm -rf %q\n' "${FINAL_LIB}"
printf '  cmd rm -rf %q\n' "${INTERMED}"
printf 'end\n'

# Default build rule

printf 'start\n'
printf '  id build\n'
printf '  req link-bin\n'
printf '  req build-lib\n'
printf '  req build-include\n'
printf 'end\n'
