// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Concrete `Box` classes
//

#ifndef _TYPE_BOX_H_
#define _TYPE_BOX_H_

#include "type/Core.h"


/** Class value for in-model class `Box`. */
extern zvalue CLS_Box;

/** Class value for in-model class `Cell`. */
extern zvalue CLS_Cell;

/** Class value for in-model class `Lazy`. */
extern zvalue CLS_Lazy;

/** Class value for in-model class `NullBox`. */
extern zvalue CLS_NullBox;

/** Class value for in-model class `Promise`. */
extern zvalue CLS_Promise;

/** Class value for in-model class `Result`. */
extern zvalue CLS_Result;

/** The sole instance of class `NullBox`. */
extern zvalue THE_NULL_BOX;

/** The sole void instance of class `Result`. */
extern zvalue THE_VOID_RESULT;

#endif
