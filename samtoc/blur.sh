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

progDir="$(abs-path .)"
baseDir="$(abs-path ..)"
projectName="${progDir##*/}"
OUT="${OUT:-${baseDir}/out}"
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
    --id=build \
    --target-dir="${FINAL_LIB}" \
    -- "${LIB_FILES[@]}"

rule copy \
    --id=build \
    --target-dir="${FINAL_BIN}" \
    --chmod=755 \
    -- "${binName}"

rule rm \
    --id=clean \
    --in-dir="${FINAL_BIN}" \
    -- "${binName}"

rule rm \
    --id=clean \
    -- "${FINAL_LIB}"
