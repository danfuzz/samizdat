// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Int.h"
#include "type/OneOff.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
zvalue get(zvalue value, zvalue key) {
    return METH_CALL(get, value, key);
}

// Documented in header.
zint get_size(zvalue value) {
    return zintFromInt(METH_CALL(get_size, value));
}

// Documented in header.
zvalue nth(zvalue value, zint index) {
    return METH_CALL(nth, value, intFromZint(index));
}

// Documented in header.
zint nthChar(zvalue value, zint index) {
    zvalue result = nth(value, index);
    return (result == NULL) ? -1 : zcharFromString(result);
}


//
// Class Definition
//

/** Initializes the module. */
MOD_INIT(OneOff) {
    MOD_USE(Value);

    SEL_INIT(cat);
    SEL_INIT(get);
    SEL_INIT(get_key);
    SEL_INIT(get_size);
    SEL_INIT(get_value);
    SEL_INIT(readResource);
    SEL_INIT(resolve);
    SEL_INIT(nth);
    SEL_INIT(toInt);
    SEL_INIT(toNumber);
    SEL_INIT(toString);
}

// Documented in header.
SEL_DEF(cat);

// Documented in header.
SEL_DEF(get);

// Documented in header.
SEL_DEF(get_key);

// Documented in header.
SEL_DEF(get_size);

// Documented in header.
SEL_DEF(get_value);

// Documented in header.
SEL_DEF(nth);

// Documented in header.
SEL_DEF(readResource);

// Documented in header.
SEL_DEF(resolve);

// Documented in header.
SEL_DEF(toInt);

// Documented in header.
SEL_DEF(toNumber);

// Documented in header.
SEL_DEF(toString);
