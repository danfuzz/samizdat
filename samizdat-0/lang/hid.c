/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/** The value `false`. Lazily initialized. */
static zvalue HID_FALSE = NULL;

/** The value `true`. Lazily initialized. */
static zvalue HID_TRUE = NULL;

/**
 * Initialize the constants (above) if necessary.
 */
static void initHidConsts(void) {
    if (HID_FALSE != NULL) {
        return;
    }

    constInit();
    HID_FALSE = datHighletFrom(STR_BOOLEAN, datIntletFromInt(0));
    HID_TRUE  = datHighletFrom(STR_BOOLEAN, datIntletFromInt(1));
}


/*
 * Exported functions
 */

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

/* Documented in header. */
bool langBoolFromBoolean(zvalue value) {
    initHidConsts();

    if (datOrder(value, HID_FALSE) == 0) {
        return false;
    } else if (datOrder(value, HID_TRUE) == 0) {
        return true;
    }

    die("Not a boolean value.");
}

/* Documented in header. */
zvalue langBooleanFromBool(bool value) {
    initHidConsts();
    return value ? HID_TRUE : HID_FALSE;
}
