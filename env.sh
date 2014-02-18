# Copyright 2014 the Samizdat Authors (Dan Bornstein et alia).
# Licensed AS IS and WITHOUT WARRANTY under the Apache License,
# Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#
# Source this file to set up for development.
#

function init-env {
    # Set up `SAMIZDAT_BASE_DIR` to point at the root directory of the source.

    local thisFile="${BASH_SOURCE[0]}"
    export SAMIZDAT_BASE_DIR="$(cd "$(dirname "${thisFile}")"; /bin/pwd -P)"

    # Set up PATH, so that `out/final/bin` is at its head. This is nontrivial
    # in an attempt to avoid having more than one `out...` in the PATH.

    local oldIfs="${IFS}"
    IFS=':'
    local -a paths=(${PATH})
    IFS="${oldIfs}"

    PATH="$(
        printf '%s' "${SAMIZDAT_BASE_DIR}/out/final/bin"
        for (( i = 0; i < ${#paths[@]}; i++ )); do
            if [[ ! ${paths[$i]} =~ /out/final/bin$ ]]; then
                printf ':%s' "${paths[$i]}"
            fi
        done
    )"
}

init-env
unset -f init-env


#
# Exported functions
#

# Calls the Blur build utility.
function blur {
    "${SAMIZDAT_BASE_DIR}/blur" "$@"
}
