// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Safe integer functions
//
// This file just serves to define the non-inline versions of all the
// functions.
//

#include "util.h"


//
// Private Definitions
//

// Documented in header.
extern bool zintCanDiv(zint x, zint y);


//
// Exported Definitions
//

// All documented in header.
extern bool zcharFromZint(zchar *result, zint value);
extern bool zintAbs(zint *result, zint x);
extern bool zintAdd(zint *result, zint x, zint y);
extern bool zintAnd(zint *result, zint x, zint y);
extern bool zintBit(zint *result, zint x, zint y);
extern zint zintBitSize(zint value);
extern bool zintDiv(zint *result, zint x, zint y);
extern bool zintDivEu(zint *result, zint x, zint y);
extern bool zintMod(zint *result, zint x, zint y);
extern bool zintModEu(zint *result, zint x, zint y);
extern bool zintMul(zint *result, zint x, zint y);
extern bool zintNeg(zint *result, zint x);
extern bool zintNot(zint *result, zint x);
extern bool zintOr(zint *result, zint x, zint y);
extern bool zintSign(zint *result, zint x);
extern bool zintShl(zint *result, zint x, zint y);
extern bool zintShr(zint *result, zint x, zint y);
extern bool zintSub(zint *result, zint x, zint y);
extern bool zintXor(zint *result, zint x, zint y);

extern bool zintSafeBitSize(zint *result, zint value);
