/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "parse.h"

#include <stddef.h>

typedef struct {
    /** token list being parsed. */
    zvalue tokens;

    /** Size of `tokens`. */
    zint size;

    /** Current read position. */
    zint at;
}
ParseState;


/*
 * Helper functions
 */

/**
 * Is the parse state at EOF?
 */
static bool isEof(ParseState *state) {
    return (state->at >= state->size);
}

/**
 * Peeks at the next token.
 */
static zvalue peek(ParseState *state) {
    return samListletGet(state->tokens, state->at);
}

/**
 * Reads the next token.
 */
static zvalue read(ParseState *state) {
    zvalue result = peek(state);
    state->at++;

    return result;
}


/*
 * Exported functions
 */

/** Documented in `parse.h`. */
zvalue parse(zvalue tokens) {
    ParseState state = { tokens, samSize(tokens), 0 };

    samDie("TODO");
}
