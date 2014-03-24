# Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
# Licensed AS IS and WITHOUT WARRANTY under the Apache License,
# Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>


#
# Argument parsing
#

if [[ $1 != '' ]]; then
    echo "Unknown option: $1" 1>&2
    exit 1
fi


#
# Main script
#

OUT="${BASE_DIR}/out"
FINAL="${OUT}/final"

INTERMED="${OUT}/intermed/${PROJECT_NAME}"
FINAL_BIN="${FINAL}/bin"
FINAL_LIB="${FINAL}/lib/${PROJECT_NAME}"
FINAL_INCLUDE="${FINAL}/include/${PROJECT_NAME}"

# This skips top-level sources, module definition files, and the `EntityMap`
# module. The last is because it its hugeness makes for a slow `samtoc` run,
# and compiling it doesn't really speed it up anyway.
SOURCE_FILES=(
    $(cd ../samlib-naif; find . \
        -type f \
        '!' '(' \
            '(' -depth 1 -name '*.sam' ')' -o \
            '(' -path '*/core.EntityMap/*' ')' \
        ')' \
        '(' -name '*.sam' -o -name '*.sam[0-9]' ')' \
        -print
    ))

# Files that are just copied as-is to the final lib directory. This is
# everything in the library source directory not covered by `SOURCE_FILES`,
# above.
EXTRA_FILES=(
    $(cd ../samlib-naif; find . \
        -type f \
        '(' \
            '(' -depth 1 -name '*.sam' ')' -o \
            '(' -path '*/core.EntityMap/*' ')' -o \
            '!' '(' -name '*.sam' -o -name '*.sam[0-9]' ')' \
        ')' \
        -print
    ))

# These are all the intermediate C source files, corresponding to original
# sources.
C_SOURCE_FILES=("${SOURCE_FILES[@]/%.sam/.c}")         # Change suffix.
C_SOURCE_FILES=("${C_SOURCE_FILES[@]/#/${INTERMED}/}") # Add directory prefix.


# Sub-rules for file copying

# Copies all non-source files (resource files, essentially) to the final
# lib directory.

rule copy \
    --id=copy-files \
    --in-dir="../samlib-naif" \
    --out-dir="${FINAL_LIB}/corelib" \
    -- "${EXTRA_FILES[@]}"

# Copies the compiled `samex` binary from `samex-naif` to the final lib
# directory.

rule copy \
    --id=copy-files \
    --in-dir="${FINAL}/lib/samex-naif" \
    --out-dir="${FINAL_LIB}" \
    -- samex

# Rules to copy each include file to the final include directory.

INCLUDE_SOURCE_BASE="../samex-naif/include"
INCLUDE_FILES=($(cd "${INCLUDE_SOURCE_BASE}"; find . -name '*.h'))

rule copy \
    --id=copy-files \
    --in-dir="${INCLUDE_SOURCE_BASE}" \
    --out-dir="${FINAL_INCLUDE}" \
    -- "${INCLUDE_FILES[@]}"


# Sub-rules for translation and compilation

# This builds up a set of all the source files that need to be converted to
# C, and processes them all in a single call to `samtoc`. Output files (C
# sources) are placed in the intermediates directory. The groups ensure that
# `samtoc` is only asked to process out-of-date sources. `--value` is used to
# pass through the relative path of the source, since that makes the `samtoc`
# call more straightforward.

groups=()
for (( i = 0; i < ${#SOURCE_FILES[@]}; i++ )); do
    inFile="${SOURCE_FILES[$i]}"
    outFile="${C_SOURCE_FILES[$i]}"
    outDir="${outFile%/*}"

    rule mkdir -- "${outDir}"

    groups+=(
        '('
        --req="${outDir}"
        --req="../samlib-naif/${inFile}"
        --target="${outFile}"
        --value="${inFile}"
        ')'
    )
done

samtocCmdStart="$(quote \
    "${OUT}/final/bin/samtoc" \
    --in-dir="../samlib-naif" \
    --out-dir="${INTERMED}" \
    --mode=interp-tree \
    --
)"

rule body \
    "${groups[@]}" \
    -- \
    --cmd='printf "Will compile: %s\n" "${VALUES[@]}"' \
    --cmd="${samtocCmdStart}"' "${VALUES[@]}"'

# Rules to compile each C source file.

for file in "${SOURCE_FILES[@]}"; do
    dir="${file%/*}"
    name="${file##*/}"

    inFile="${INTERMED}/${dir}/${name/%.sam/.c}"
    outDir="${FINAL_LIB}/corelib/${dir}"
    outFile="${outDir}/${name/%.sam/.samb}"

    rule mkdir -- "${outDir}"

    rule body \
        --id=compile-libs \
        --req="${inFile}" \
        --req="${outDir}" \
        --target="${outFile}" \
        --msg="Compile: ${file#./}" \
        --cmd="$(quote "${FINAL_BIN}/compile-samex-addon" \
            --runtime=naif --output="${outFile}" "${inFile}")"
done

# Default build rules

rule body \
    --id=external-reqs \
    --build-in-dir="../samtoc"

rule body \
    --id=build \
    --req-id=external-reqs \
    --req-id=compile-libs \
    --req-id=copy-files

# Rules for cleaning

rule rm \
    --id=clean \
    -- \
    "${FINAL_LIB}" "${FINAL_INCLUDE}"
