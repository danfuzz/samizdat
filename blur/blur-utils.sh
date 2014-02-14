# Copyright 2014 the Samizdat Authors (Dan Bornstein et alia).
# Licensed AS IS and WITHOUT WARRANTY under the Apache License,
# Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#
# Helper library. Meant to be included via `.` command.
#


#
# OS detection
#

# OS flavor; either `bsd` or `linux`.
OS_FLAVOR=linux

if ! ls --version > /dev/null 2>&1; then
    OS_FLAVOR=bsd
fi


#
# Exported functions
#

# Mangles an arbitrary string into a valid variable name.
function mangleString {
    local s="$1"

    printf '__'

    while [[ ${s} != '' ]]; do
        # Emit a chunk of valid characters (if any).
        local chunk="${s%%[^A-Za-z0-9]*}"
        if [[ ${chunk} != '' ]]; then
            printf '%s' "${chunk}"
            s="${s:${#chunk}}"
        fi

        # Emit the next character (if any) as an escape.
        if [[ ${s} != '' ]]; then
            local c="${s:0:1}"
            printf '_%02x' "'${c}"
            s="${s:1}"
        fi
    done
}

# Gets the modification time of the given file as seconds since the Unix
# Epoch.
function modTime {
    local file="$1"

    if [[ ${OS_FLAVOR} == bsd ]]; then
        stat -f %m "${file}"
    else
        stat --format=%Y "${file}"
    fi
}

# Makes an absolute path out of the given path, which might be either absolute
# or relative. This does not resolve symlinks but does flatten away `.` and
# `..` components.
function absPath {
    local path="$1"

    if [[ ! ${path} =~ ^/ ]]; then
        path="${PWD}/${path}"
    fi

    local result=() at=0
    while [[ ${path} != '' ]]; do
        if [[ ${path} =~ ^// ]]; then
            path="${path:1}"
        elif [[ ${path} =~ ^/\./ ]]; then
            path="${path:2}"
        elif [[ ${path} == '/.' ]]; then
            path=''
        elif [[ ${path} =~ ^/\.\./ ]]; then
            if (( at > 0 )); then
                (( at-- ))
            fi
            path="${path:3}"
        elif [[ ${path} == '/..' ]]; then
            if (( at > 0 )); then
                (( at-- ))
            fi
            path=''
        elif [[ ${path} == '/' ]]; then
            result[$at]=''
            path=''
            (( at++ ))
        else
            [[ ${path} =~ /([^/]*)(.*) ]]
            result[$at]="${BASH_REMATCH[1]}"
            path="${BASH_REMATCH[2]}"
            (( at++ ))
        fi
    done

    printf '/%s' "${result[@]}"
    printf '\n'
}

# Unquotes the given string.
function unquote {
    local s="$1"

    if [[ $s == "''" ]]; then
        # Empty string.
        return
    fi

    if [[ $s =~ ^"$'"(.*)"'"$ ]]; then
        # Remove `$'...'` cladding.
        s="${BASH_REMATCH[1]}"
    fi

    if [[ ! $s =~ '\' ]]; then
        # Easy out: No escapes.
        printf '%s' "$s"
        return
    fi

    local i result=''

    for (( i = 0; i < ${#s}; i++ )); do
        local c="${s:$i:1}"
        if [[ $c == '\' ]]; then
            ((i++))
            c="${s:$i:1}"
            case "$c" in
                ('n') c=$'\n' ;;
                ('r') c=$'\r' ;;
                ('t') c=$'\t' ;;
                (' '|'"'|"'") ;; # Pass through as-is.
                (*)
                    echo "Unknown character escape: ${c}" 1>&2
                    exit 1
                    ;;
            esac
        fi
        result+="$c"
    done

    printf '%s' "${result}"
}

# Combines `unquote` and `absPath`.
function unquoteAbs {
    absPath "$(unquote "$1")"
}


#
# Pass-through to external scripts
#

# Emits a file copy rule.
function rule-copy {
    "${BLUR_DIR}/rule-copy" "$@"
}

# Emits a directory creation rule.
function rule-mkdir {
    "${BLUR_DIR}/rule-mkdir" "$@"
}

# Like `rule-mkdir` but keeps track of what rules have been emitted, and
# only ever emits once per unique directory name.
MKDIRS=()
function rule-mkdir-once {
    local dir="$1"
    local i

    for (( i = 0; i < ${#MKDIRS[@]}; i++ )); do
        if [[ ${dir} == ${MKDIRS[$i]} ]]; then
            return
        fi
    done

    rule-mkdir "${dir}"
    MKDIRS+=("${dir}")
}


#
# Directory setup. This has to be done after `absPath` is defined.
#

BLUR_DIR="$(absPath "${BASH_SOURCE[0]%/*}")"
