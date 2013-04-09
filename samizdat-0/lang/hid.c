/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/** The value `null`. Lazily initialized. */
static zvalue HID_NULL = NULL;

/** The value `false`. Lazily initialized. */
static zvalue HID_FALSE = NULL;

/** The value `true`. Lazily initialized. */
static zvalue HID_TRUE = NULL;

/**
 * Initialize the constants (above) if necessary.
 */
static void initHidConsts(void) {
    if (HID_NULL != NULL) {
        return;
    }

    constsInit();
    HID_NULL  = TOK_NULL;
    HID_FALSE = hidPutValue(TOK_BOOLEAN, datIntletFromInt(0));
    HID_TRUE  = hidPutValue(TOK_BOOLEAN, datIntletFromInt(1));
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue hidPutValue(zvalue token, zvalue value) {
    return datMapletPut(token, STR_VALUE, value);
}

/* Documented in header. */
zvalue hidType(zvalue value) {
    return datMapletGet(value, STR_TYPE);
}

/* Documented in header. */
zvalue hidValue(zvalue value) {
    return datMapletGet(value, STR_VALUE);
}

/* Documented in header. */
bool hidHasType(zvalue value, zvalue type) {
    return (datCompare(hidType(value), type) == 0);
}

/* Documented in header. */
void hidAssertType(zvalue value, zvalue type) {
    if (!hidHasType(value, type)) {
        die("Type mismatch.");
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue langNull(void) {
    initHidConsts();
    return HID_NULL;
}

/* Documented in header. */
zvalue langFalse(void) {
    initHidConsts();
    return HID_FALSE;
}

/* Documented in header. */
zvalue langTrue(void) {
    initHidConsts();
    return HID_TRUE;
}
