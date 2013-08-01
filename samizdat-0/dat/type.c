/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/** Next type sequence number to assign. */
static zint theNextSeqNum = 0;


/*
 * Module functions
 */

/* Documented in header. */
zvalue datTypeFromZtype(ztype type) {
    zvalue result = type->nameValue;

    if (result == NULL) {
        result = datStringFromUtf8(-1, type->name);
        ((DatType *) type)->nameValue = result;  // Cast to discard `const`.
        datImmortalize(result);
    }

    return result;
}

/* Documented in header. */
zint datTypeSeqNum(ztype type) {
    zint compl = type->seqNumCompl;

    if (compl == 0) {
        if (theNextSeqNum == DAT_MAX_TYPES) {
            die("Too many types!");
        }

        compl = ~theNextSeqNum;
        ((DatType *) type)->seqNumCompl = compl;  // Cast to discard `const`.
        theNextSeqNum++;
    }

    return ~compl;
}


/*
 * Exported functions
 */

/* Documented in header. */
bool datCoreTypeIs(zvalue value, ztype type) {
    return value->type == type;
}
