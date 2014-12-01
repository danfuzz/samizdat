// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Int.h"

#include "langnode.h"


//
// Exported functions
//

// Documented in spec.
bool canYieldVoid(zvalue node) {
    switch (recordEvalType(node)) {
        case EVAL_apply: { return true;  }
        case EVAL_call:  { return true;  }
        case EVAL_fetch: { return true;  }
        case EVAL_maybe: { return true;  }
        case EVAL_store: { return true;  }
        case EVAL_void:  { return true;  }
        default:         { return false; }
    }
}

// Documented in spec.
zvalue formalsMaxArgs(zvalue formals) {
    zint maxArgs = 0;
    zint sz = get_size(formals);

    for (zint i = 0; i < sz; i++) {
        zvalue one = cm_nth(formals, i);
        zvalue repeat = cm_get(one, SYM(repeat));
        if (valEqNullOk(repeat, SYM(CH_STAR))
            || valEqNullOk(repeat, SYM(CH_PLUS))) {
            maxArgs = -1;
            break;
        }
        maxArgs++;
    }

    return intFromZint(maxArgs);
}

// Documented in spec.
zvalue formalsMinArgs(zvalue formals) {
    zint minArgs = 0;
    zint sz = get_size(formals);

    for (zint i = 0; i < sz; i++) {
        zvalue one = cm_nth(formals, i);
        zvalue repeat = cm_get(one, SYM(repeat));
        if (!(valEqNullOk(repeat, SYM(CH_QMARK))
              || valEqNullOk(repeat, SYM(CH_STAR)))) {
            minArgs++;
        }
    }

    return intFromZint(minArgs);
}

// Documented in spec.
bool isExpression(zvalue node) {
    switch (recordEvalType(node)) {
        case EVAL_apply:   { return true;  }
        case EVAL_call:    { return true;  }
        case EVAL_closure: { return true;  }
        case EVAL_fetch:   { return true;  }
        case EVAL_literal: { return true;  }
        case EVAL_noYield: { return true;  }
        case EVAL_store:   { return true;  }
        case EVAL_varRef:  { return true;  }
        default:           { return false; }
    }
}
