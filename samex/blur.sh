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

binNames=(
    "bin/samex"
    "bin/compile-samex-addon"
    "helper/find-samex"
)

rule copy \
    --out-dir="${FINAL}" \
    --chmod=755 \
    -- "${binNames[@]}"

rule body \
    --id=build \
    "${binNames[@]/#/--req=${FINAL}/}"

rule rm \
    --id=clean \
    --in-dir="${FINAL}" \
    -- "${binNames[@]}"
