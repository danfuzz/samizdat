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

OUT="${PROJECT_DIR}/out"

rule body \
    --id=build \
    --build-in-dir="installer" \
    --build-in-dir="samex-tot"

rule rm \
    --id=clean \
    -- "${OUT}"
