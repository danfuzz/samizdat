/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/**
 * Gets a pointer to the token's info.
 */
static TokenInfo *tokenInfo(zvalue token) {
    return &((DatToken *) token)->info;
}

/**
 * Allocates and initializes a token, without doing error-checking
 * on the arguments.
 */
static zvalue newToken(zvalue type, zvalue value) {
    zvalue result = datAllocValue(DAT_VALUE, 0, sizeof(TokenInfo));
    TokenInfo *info = tokenInfo(result);

    result->size = (value == NULL) ? 0 : 1;
    info->type = type;
    info->value = value;
    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
bool datTokenEq(zvalue v1, zvalue v2) {
    TokenInfo *info1 = tokenInfo(v1);
    TokenInfo *info2 = tokenInfo(v2);

    if (!datEq(info1->type, info2->type)) {
        return false;
    }

    if (info1->value == NULL) {
        return (info2->value == NULL);
    } else if (info2->value == NULL) {
        return false;
    } else {
        return datEq(info1->value, info2->value);
    }
}

/* Documented in header. */
zorder datTokenOrder(zvalue v1, zvalue v2) {
    TokenInfo *info1 = tokenInfo(v1);
    TokenInfo *info2 = tokenInfo(v2);

    zorder result = datOrder(info1->type, info2->type);

    if (result != ZSAME) {
        return result;
    } else if (info1->value == NULL) {
        return (info2->value == NULL) ? ZSAME : ZLESS;
    } else if (info2->value == NULL) {
        return ZMORE;
    } else {
        return datOrder(info1->value, info2->value);
    }
}

/* Documented in header. */
void datTokenMark(zvalue value) {
    TokenInfo *info = tokenInfo(value);

    datMark(info->type);
    if (info->value != NULL) {
        datMark(info->value);
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datTokenFrom(zvalue type, zvalue value) {
    datAssertValid(type);

    if (value != NULL) {
        datAssertValid(value);
    }

    return newToken(type, value);
}

/* Documented in header. */
zvalue datTokenType(zvalue token) {
    datAssertToken(token);
    return tokenInfo(token)->type;
}

/* Documented in header. */
bool datTokenTypeIs(zvalue token, zvalue type) {
    return datEq(datTokenType(token), type);
}

/* Documented in header. */
zvalue datTokenValue(zvalue token) {
    datAssertToken(token);
    return tokenInfo(token)->value;
}
