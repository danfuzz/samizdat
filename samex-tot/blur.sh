# Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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

LIB_SOURCE_DIR='../samlib-naif'

# Names of all the modules defined in the core library.
MODULE_NAMES=(
    $(
        cd "${LIB_SOURCE_DIR}/modules"
        find . \
            -maxdepth 1 -mindepth 1 \
            '(' \
                '(' -type d -print ')' -o \
                '(' -type f -name '*.saminfo' -print ')' \
            ')' \
        | awk '
        {
            name = substr($0, 3);
            if (match(name, /\.saminfo$/)) {
                name = substr(name, 1, length(name) - 8);
            }
            print name;
        }'
    ))

# Names of all the source files in the core library.
SOURCE_FILES=(
    $(cd "${LIB_SOURCE_DIR}"; find . \
        -type f \
        -name '*.sam' \
        -print
    ))

# Files that are just copied as-is to the final lib directory. This is
# everything in the library source directory not covered by `SOURCE_FILES`,
# above.
EXTRA_FILES=(
    $(cd "${LIB_SOURCE_DIR}"; find . \
        -type f \
        '(' '!' -name '*.sam' ')' \
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
    --in-dir="${LIB_SOURCE_DIR}" \
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
        --req="${LIB_SOURCE_DIR}/${inFile}"
        --target="${outFile}"
        --value="${inFile}"
        ')'
    )
done

samtocCmdStart="$(quote \
    "${OUT}/final/bin/samtoc" \
    --in-dir="${LIB_SOURCE_DIR}" \
    --out-dir="${INTERMED}" \
    --no-core-dir \
    --dir-selection \
    --mode=interp-tree \
    --
)"

rule body \
    "${groups[@]}" \
    -- \
    --cmd='printf "Will compile: %s\n" "${VALUES[@]}"' \
    --cmd="${samtocCmdStart}"' . "${VALUES[@]}"'

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

# Similar to the C compilation rules above, this set of rules arranges for
# linkage metainfo to be produced.

inDir="${LIB_SOURCE_DIR}/modules"
outDir="${FINAL_INCLUDE}/modules"

rule mkdir -- "${outDir}"

groups=()
for name in "${MODULE_NAMES[@]}"; do
    inFile="${inDir}/${name}"
    outFile="${outDir}/${name}.saminfo"

    if [[ -r "${inFile}.saminfo" ]]; then
        # It's a prefab info file. Arrange for it to get copied.
        rule copy \
            --id=make-linkage \
            --req="${outDir}" \
            --in-dir="${inDir}" \
            --out-dir="${outDir}" \
            -- "${name}.saminfo"
    else
        # It's a regular module.
        groups+=(
            '('
            --req="${outDir}"
            --req="${inFile}/main.sam"
            --target="${outFile}"
            --value="./${name}"
            ')'
        )
    fi
done

samtocCmdStart="$(quote \
    "${OUT}/final/bin/samtoc" \
    --in-dir="${inDir}" \
    --out-dir="${outDir}" \
    --no-core-dir \
    --dir-selection \
    --mode=linkage \
    --
)"

rule body \
    --id=make-linkage \
    "${groups[@]}" \
    -- \
    --cmd='printf "Need linkage: %s\n" "${VALUES[@]}"' \
    --cmd="${samtocCmdStart}"' . "${VALUES[@]}"'


# Default build rules

rule body \
    --id=external-reqs \
    --build-in-dir="../samtoc"

rule body \
    --id=build \
    --req-id=external-reqs \
    --req-id=compile-libs \
    --req-id=copy-files \
    --req-id=make-linkage

# Rules for cleaning

rule rm \
    --id=clean \
    -- \
    "${INTERMED}" "${FINAL_LIB}" "${FINAL_INCLUDE}"
