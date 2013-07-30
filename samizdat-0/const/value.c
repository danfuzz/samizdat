/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "util.h"


/*
 * Exported functions
 */

/* Documented in header. */
zvalue constDataOf(zvalue value) {
    if (datTypeIs(value, DAT_Deriv)) {
        return datDerivData(value);
    } else {
        return value;
    }
}

/* Documented in header. */
zvalue constCoreTypeName(zvalue value) {
    switch (datType(value)) {
        case DAT_DERIV:   return STR_CAP_DERIV;
        case DAT_INT:     return STR_CAP_INT;
        case DAT_STRING:  return STR_CAP_STRING;
        case DAT_LIST:    return STR_CAP_LIST;
        case DAT_MAP:     return STR_CAP_MAP;
        case DAT_UNIQLET: return STR_CAP_UNIQLET;
        default: {
            die("Invalid core type (shouldn't happen): %d", datType(value));
        }
    }
}

/* Documented in header. */
bool constIsCoreTypeName(zvalue value) {
    // Switch on the size to avoid too many comparisons.
    switch (datSize(value)) {
        case 3: {
            return datEq(value, STR_CAP_INT) || datEq(value, STR_CAP_MAP);
        }
        case 4: {
            return datEq(value, STR_CAP_LIST);
        }
        case 5: {
            return datEq(value, STR_CAP_DERIV);
        }
        case 6: {
            return datEq(value, STR_CAP_STRING);
        }
        case 7: {
            return datEq(value, STR_CAP_UNIQLET);
        }
        default: {
            return false;
        }
    }
}

/* Documented in header. */
bool constTypeIs(zvalue value, zvalue type) {
    return datEq(constTypeOf(value), type);
}

/* Documented in header. */
zvalue constTypeOf(zvalue value) {
    if (datTypeIs(value, DAT_Deriv)) {
        return datDerivType(value);
    } else {
        return constCoreTypeName(value);
    }
}
