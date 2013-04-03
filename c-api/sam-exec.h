/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Execution API for low-level (C) code
 */

#ifndef SAM_EXEC_H
#define SAM_EXEC_H

#include "sam-data.h"

/**
 * Magic function provider. Users of `samExecute()` must provide an
 * implementation of this type to allow code to escape from a mode of
 * pure computation.
 */
typedef zvalue zmagic(zvalue uniqlet, zint argCount, const zvalue *args);

/**
 * Executes the given code, using the given global environment and
 * magic provider. Returns an environment representing the effects
 * caused by the code. In addition, this can cause arbitrary
 * calls to be performed on the magic provider (which can respond
 * to those calls as it sees fit).
 *
 * An environment is a maplet representing global variable bindings.
 * Bindings of the form `@{@"type": @"function", @"value": v}` are
 * taken to be function bindings, where `v` must be code of the same
 * form as is taken by this function.
 *
 * Code may either be a maplet value (details described elsewhere) or
 * a uniqlet that is bound as a function by the given magic
 * provider.
 */
zvalue samExecute(zvalue environment, zmagic *magic, zvalue code);

#endif
