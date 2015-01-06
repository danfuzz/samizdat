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

files=(
    NAME.txt
    VERSION.txt
    "${PROJECT_DIR}/install"
)

rule copy \
    --id=copy-files \
    --out-dir="${FINAL}" \
    --chmod=755 \
    -- install

rule copy \
    --id=copy-files \
    --in-dir="${BASE_DIR}" \
    --out-dir="${FINAL}" \
    --chmod=755 \
    -- NAME.txt VERSION.txt

rule body \
    --id=build \
    --req-id=copy-files

rule rm \
    --id=clean \
    --in-dir="${FINAL}" \
    -- NAME.txt VERSION.txt install
