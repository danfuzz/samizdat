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

binNames=(
    "bin/samex"
    "bin/compile-samex-addon"
    "helper/find-samex"
)

rule body \
    --group-start \
        --req=bin/samex \
        --target="${FINAL}/bin/samex" \
    --group-end \
    --group-start \
        --req=bin/compile-samex-addon \
        --target="${FINAL}/bin/compile-samex-addon" \
    --group-end \
    --group-start \
        --req=helper/find-samex \
        --target="${FINAL}/helper/find-samex" \
    --group-end \
    --msg=$'Stuff\nis\nstale.' \
    --cmd='printf "stale: %s\n" "${STALE_TARGETS[@]}"'

rule copy \
    --id=build \
    --out-dir="${FINAL}" \
    --chmod=755 \
    -- "${binNames[@]}"

rule rm \
    --id=clean \
    --in-dir="${FINAL}" \
    -- "${binNames[@]}"
