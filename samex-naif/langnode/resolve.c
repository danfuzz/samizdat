// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/List.h"
#include "type/SymbolTable.h"
#include "util.h"

#include "langnode.h"


//
// Exported functions
//

// Documented in spec.
zvalue resolveImport(zvalue node, zvalue resolveFn) {
    if (nodeRecTypeIs(node, NODE_importResource)) {
        // No conversion, just validation. TODO: Validate.
        //
        // **Note:** This clause is at the top so as to avoid the call to
        // `resolveFn()` below, which is inappropriate to do on resources.
        return node;
    }

    zvalue resolved = EMPTY_SYMBOL_TABLE;
    if (resolveFn != NULL) {
        zvalue source = cm_get(node, SYM(source));
        resolved = FUN_CALL(resolveFn, source);
        if (resolved == NULL) {
            die("Could not resolve import.");
        } else if (!nodeRecTypeIs(resolved, NODE_module)) {
            die("Invalid resolution result (not a `@module`)");
        }
    }

    switch (nodeRecType(node)) {
        case NODE_importModule: {
            // No conversion, just validation (done above).
            return node;
        }
        case NODE_importModuleSelection: {
            // Get the exports. When given a `NULL` `resolveFn`, this acts as
            // if all sources resolve to an empty export map, and hence this
            // node won't bind anything.
            zvalue exports = EMPTY_LIST;
            zvalue info = cm_get(resolved, SYM(info));
            if (info != NULL) {
                exports = cm_get(info, SYM(exports));
            }

            zvalue select = cm_get(node, SYM(select));
            if (select != NULL) {
                // The selection is specified. So no modification needs to be
                // done to the node, just validation, including of the import
                // in general (above) and the particular selection (here).
                zint size = get_size(select);
                for (zint i = 0; i < size; i++) {
                    zvalue one = cm_nth(select, i);
                    if (cm_get(exports, one) == NULL) {
                        die("Could not resolve import selection.");
                    }
                }
                return node;
            } else {
                // It's a wildcard select.
                select = METH_CALL(exports, keyList);
                return cm_cat(node, cm_new_SymbolTable(SYM(select), select));
            }
        }
        default: {
            die("Bad node type for `resolveImport`.");
        }
    }
}

// Documented in spec.
zvalue withResolvedImports(zvalue node, zvalue resolveFn) {
    zvalue rawStatements = cm_get(node, SYM(statements));
    zarray arr = zarrayFromList(rawStatements);
    zvalue elems[arr.size];

    for (zint i = 0; i < arr.size; i++) {
        zvalue s = elems[i] = arr.elems[i];
        bool exported = false;
        zvalue defNode = s;

        if (nodeRecTypeIs(s, NODE_export)) {
            exported = true;
            defNode = cm_get(s, SYM(value));
        }

        switch (nodeRecType(defNode)) {
            case NODE_importModule:
            case NODE_importModuleSelection:
            case NODE_importResource: {
                zvalue resolved = resolveImport(defNode, resolveFn);
                elems[i] = exported ? makeExport(resolved) : resolved;
            }
            default: {
                // The rest of the types are intentionally left un-handled.
                break;
            }
        }
    }

    zvalue converted = listFromZarray((zarray) {arr.size, elems});

    return cm_cat(node, cm_new_SymbolTable(SYM(statements), converted));
}
