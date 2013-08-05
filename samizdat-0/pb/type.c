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
zint indexFromZtype(ztype type) {
    zint compl = type->seqNumCompl;

    if (compl == 0) {
        if (theNextSeqNum == DAT_MAX_TYPES) {
            die("Too many types!");
        }

        compl = ~theNextSeqNum;
        ((PbType *) type)->seqNumCompl = compl;  // Cast to discard `const`.
        theNextSeqNum++;
    }

    return ~compl;
}

/* Documented in header. */
zvalue typeFromZtype(ztype type) {
    zvalue result = type->nameValue;

    if (result == NULL) {
        result = stringFromUtf8(-1, type->name);
        ((PbType *) type)->nameValue = result;  // Cast to discard `const`.
        pbImmortalize(result);
    }

    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
bool pbCoreTypeIs(zvalue value, ztype type) {
    return value->type == type;
}
