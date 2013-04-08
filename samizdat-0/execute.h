/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Execution of Samizdat Layer 0 parse trees
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include "function.h"

/**
 * Executes the given code, using the given global environment and
 * function registry. Returns an environment representing the effects
 * caused by the code on the originally passed environment. In
 * addition, this can cause arbitrary calls to be performed on and via
 * the function registry (which can respond to those calls as it sees
 * fit).
 *
 * An environment is a maplet representing variable bindings.
 * Bindings to uniqlets are considered to be callable as functions,
 * with such calling being done via the function registry.
 *
 * The given `code` must be a `statements` node, such as would have
 * been returned from `parse()`.
 */
zvalue execute(zvalue environment, zfunreg reg, zvalue code);


#endif
