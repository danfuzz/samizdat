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

binName="samtoc"

INTERMED="${OUT}/intermed/${PROJECT_NAME}"
FINAL_BIN="${FINAL}/bin"
FINAL_LIB="${FINAL}/lib/${binName}"

# Used for linking.
SAMLIB_CORE_DIR="${BASE_DIR}/samlib-naif"

SOURCE_FILES=($(find . -type f -name '*.sam'))
EXTRA_FILES=($(find modules -type f '!' -name '*.sam'))

# These are all the intermediate C source files, corresponding to original
# sources.
C_SOURCE_FILES=("${SOURCE_FILES[@]/%.sam/.c}")         # Change suffix.
C_SOURCE_FILES=("${C_SOURCE_FILES[@]/#/${INTERMED}/}") # Add directory prefix.

# Sub-rules for file copying

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

# Sub-rules for translation and compilation

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

# This sets up the `samtoc` command to run directly out of its source on
# a new build, but use the already-built binary for subsequent runs. If a
# change introduces an error in "tree" mode, then this can cause a cascading
# failure; this can be corrected by removing the compiled output of `samtoc`.
if [[ -x "${FINAL_BIN}/samtoc" ]]; then
    samtocCmdStart="$(quote "${FINAL_BIN}/samtoc")"
else
    samtocCmdStart="$(quote "${FINAL_BIN}/samex" .)"
fi
samtocCmdStart+=" $(quote \
    --out-dir="${INTERMED}" \
    --mode=interp-tree \
    --dir-selection \
    --core-dir="${SAMLIB_CORE_DIR}")"

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
    outDir="${FINAL_LIB}/${dir}"
    outFile="${outDir}/${name/%.sam/.samb}"

    rule mkdir -- "${outDir}"

    rule body \
        --id=compile-self \
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
    --build-in-dir="../samex-naif"

rule body \
    --id=build \
    --req-id=external-reqs \
    --req-id=copy-files \
    --req-id=compile-self

# Rules for cleaning

rule rm \
    --id=clean \
    -- \
    "${FINAL_LIB}" \
    "${FINAL_BIN}/${binName}" \
    "${INTERMED}"
