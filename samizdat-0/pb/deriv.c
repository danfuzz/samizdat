/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Builtin.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Value.h"
#include "zlimits.h"


/*
 * Private Definitions
 */

/**
 * Payload data for all Deriv values.
 */
typedef struct {
    /** Data payload. */
    zvalue data;
} DerivInfo;

/**
 * Gets the info of a derived value.
 */
static DerivInfo *getInfo(zvalue value) {
    return (DerivInfo *) pbPayload(value);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue valDataOf(zvalue value, zvalue secret) {
    zvalue type = value->type;

    if (typeIsDerived(type) && typeSecretIs(type, secret)) {
        return getInfo(value)->data;
    } else {
        return NULL;
    }
}

/* Documented in header. */
zvalue makeValue(zvalue type, zvalue data, zvalue secret) {
    assertValid(type);
    assertValidOrNull(data);
    assertValidOrNull(secret);

    // Make sure the secrets match. In the case of a transparent type,
    // this both converts to a `Type` and checks that `secret` is `NULL`.
    zvalue trueType = typeFromTypeAndSecret(type, secret);

    if (trueType == NULL) {
        die("Attempt to create derived value with incorrect secret.");
    }

    zvalue result = pbAllocValue(trueType, sizeof(DerivInfo));
    ((DerivInfo *) pbPayload(result))->data = data;

    return result;
}

/* Documented in header. */
zvalue makeTransValue(zvalue type, zvalue data) {
    return makeValue(type, data, NULL);
}


/*
 * Type Definition
 *
 * **Note:** This isn't the usual form of type definition, since these
 * methods are bound on many types.
 */

/** Builtin for `Deriv:gcMark`. */
static zvalue BI_Deriv_gcMark = NULL;

/** Builtin for `Deriv:totEq`. */
static zvalue BI_Deriv_totEq = NULL;

/** Builtin for `Deriv:totOrder`. */
static zvalue BI_Deriv_totOrder = NULL;

/* Documented in header. */
METH_IMPL(Deriv, gcMark) {
    zvalue value = args[0];
    pbMark(getInfo(value)->data);
    return NULL;
}

/* Documented in header. */
METH_IMPL(Deriv, totEq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return valEq(getInfo(v1)->data, getInfo(v2)->data) ? v2 : NULL;
}

/* Documented in header. */
METH_IMPL(Deriv, totOrder) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zvalue data1 = getInfo(v1)->data;
    zvalue data2 = getInfo(v2)->data;

    if (data1 == NULL) {
        return (data2 == NULL) ? INT_0 : INT_NEG1;
    } else if (data2 == NULL) {
        return INT_1;
    } else {
        return intFromZint(valOrder(data1, data2));
    }
}

/* Documented in header. */
void derivBind(zvalue type) {
    genericBind(GFN_gcMark,   type, BI_Deriv_gcMark);
    genericBind(GFN_totEq,    type, BI_Deriv_totEq);
    genericBind(GFN_totOrder, type, BI_Deriv_totOrder);
}

/* Documented in header. */
void pbBindDeriv(void) {
    BI_Deriv_gcMark = makeBuiltin(1, 1, METH_NAME(Deriv, gcMark),
        stringFromUtf8(-1, "Deriv:gcMark"));
    pbImmortalize(BI_Deriv_gcMark);

    BI_Deriv_totEq = makeBuiltin(2, 2, METH_NAME(Deriv, totEq),
        stringFromUtf8(-1, "Deriv:totEq"));
    pbImmortalize(BI_Deriv_totEq);

    BI_Deriv_totOrder = makeBuiltin(2, 2, METH_NAME(Deriv, totOrder),
        stringFromUtf8(-1, "Deriv:totOrder"));
    pbImmortalize(BI_Deriv_totOrder);
}
