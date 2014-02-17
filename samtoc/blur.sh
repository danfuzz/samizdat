# Copyright 2014 the Samizdat Authors (Dan Bornstein et alia).
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

binName="samtoc"

INTERMED="${OUT}/intermed/${PROJECT_NAME}"
FINAL_BIN="${FINAL}/bin"
FINAL_LIB="${FINAL}/lib/${binName}"

SOURCE_FILES=($(find . -type f -name '*.sam'))
EXTRA_FILES=($(find modules -type f '!' -name '*.sam'))

# These are all the intermediate C source files, corresponding to original
# sources.
C_SOURCE_FILES=("${SOURCE_FILES[@]/%.sam/.c}")         # Change suffix.
C_SOURCE_FILES=("${C_SOURCE_FILES[@]/#/${INTERMED}/}") # Add directory prefix.


# Copies the wrapper script into place.
rule copy \
    --id=copy-files \
    --out-dir="${FINAL_BIN}" \
    --chmod=755 \
    -- "${binName}"

# Copies all non-source files (resource files, essentially) to the final
# lib directory.
rule copy \
    --id=copy-files \
    --out-dir="${FINAL_LIB}" \
    -- "${EXTRA_FILES[@]}"

# TEMP: Copy all source files to final.
rule copy \
    --id=copy-files \
    --out-dir="${FINAL_LIB}" \
    -- "${SOURCE_FILES[@]}"

# Runs `samtoc` out of its source directory, in order to process its own
# files. Output files (C sources) are placed in the intermediates directory.
# The groups ensure that `samtoc` is only asked to process out-of-date
# sources. `--value` is used to pass through the relative path of the source,
# since that makes the `samtoc` call more straightforward.

groups=()
for (( i = 0; i < ${#SOURCE_FILES[@]}; i++ )); do
    inFile="${SOURCE_FILES[$i]}"
    outFile="${C_SOURCE_FILES[$i]}"
    outDir="${outFile%/*}"

    rule mkdir -- "${outDir}"

    groups+=(
        '('
        --req="${outDir}"
        --req="${inFile}"
        --target="${outFile}"
        --value="${inFile}"
        ')'
    )
done

rule body \
    --id=process-self \
    "${groups[@]}" \
    -- \
    --cmd='printf "=== req %s\n" ${NEW_REQS[@]}' \
    --cmd='printf "=== trg %s\n" ${STALE_TARGETS[@]}' \
    --cmd='printf "=== val %s\n" ${VALUES[@]}'

# Default build rules

rule body \
    --id=external-reqs \
    --build-in-dir="../samex-naif"

rule body \
    --id=build \
    --req-id=external-reqs \
    --req-id=copy-files \
    --req-id=process-self

# Rules for cleaning

rule rm \
    --id=clean \
    -- \
    "${FINAL_LIB}" \
    "${FINAL_BIN}/${binName}"
