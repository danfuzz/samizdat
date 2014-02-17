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

OUT="${BASE_DIR}/out}"
FINAL="${OUT}/final"

binName="samtoc"
FINAL_BIN="${FINAL}/bin"
FINAL_LIB="${FINAL}/lib/${binName}"

LIB_FILES=(
    main.sam
    module.sam
    $(find modules -type f)
)

rule copy \
    --id=copy-files \
    --out-dir="${FINAL_LIB}" \
    -- "${LIB_FILES[@]}"

rule copy \
    --id=copy-files \
    --out-dir="${FINAL_BIN}" \
    --chmod=755 \
    -- "${binName}"


# Default build rules

rule body \
    --id=external-reqs \
    --build-in-dir="../samex-naif"

rule body \
    --id=build \
    --req-id=external-reqs\
    --req-id=copy-files

# Rules for cleaning

rule rm \
    --id=clean \
    -- \
    "${FINAL_LIB}" \
    "${FINAL_BIN}/${binName}"
