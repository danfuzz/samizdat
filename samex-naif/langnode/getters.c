// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/List.h"
#include "type/String.h"
#include "util.h"

#include "langnode.h"


//
// Private functions
//

/** Splits a string into a list, separating at the given character. */
static zvalue splitAtChar(zvalue string, zvalue chString) {
    zchar ch = zcharFromString(chString);
    zstring s = zstringFromString(string);
    zvalue result[s.size + 1];
    zint resultAt = 0;

    for (zint at = 0; at <= s.size; /*at*/) {
        zint endAt = at;
        while ((endAt < s.size) && s.chars[endAt] != ch) {
            endAt++;
        }

        result[resultAt] =
            stringFromZstring((zstring) {endAt - at, &s.chars[at]});
        resultAt++;
        at = endAt + 1;
    }

    return listFromZarray((zarray) {resultAt, result});
}


//
// Exported functions
//

// Documented in spec.
zvalue get_baseName(zvalue source) {
    switch (nodeRecType(source)) {
        case NODE_external: {
            zvalue components =
                splitAtChar(cm_get(source, SYM(name)), STR_CH_DOT);
            return cm_nth(components, get_size(components) - 1);
        }
        case NODE_internal: {
            zvalue components =
                splitAtChar(cm_get(source, SYM(name)), STR_CH_SLASH);
            zvalue last = cm_nth(components, get_size(components) - 1);
            zvalue parts = splitAtChar(last, STR_CH_DOT);
            return cm_nth(parts, 0);
        }
        default: {
            die("Bad type for `get_baseName`.");
        }
    }
}

// Documented in spec.
zvalue get_definedNames(zvalue node) {
    switch (nodeRecType(node)) {
        case NODE_export: {
            return get_definedNames(cm_get(node, SYM(value)));
        }
        case NODE_importModule:
        case NODE_importResource:
        case NODE_varDef: {
            return cm_new_List(cm_get(node, SYM(name)));
        }
        case NODE_importModuleSelection: {
            zvalue select = cm_get(node, SYM(select));
            if (select == NULL) {
                die("Cannot call `get_definedNames` on unresolved import.");
            }

            zvalue prefix = cm_get(node, SYM(prefix));
            if (prefix != NULL) {
                zarray arr = zarrayFromList(select);
                zvalue elems[arr.size];

                for (zint i = 0; i < arr.size; i++) {
                    elems[i] = cm_cat(prefix, arr.elems[i]);
                }

                return listFromZarray((zarray) {arr.size, elems});
            } else {
                return select;
            }
        }
        default: {
            return EMPTY_LIST;
        }
    }
}
