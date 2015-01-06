// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Cmp.h"
#include "type/Int.h"

#include "langnode.h"


//
// Exported functions
//

// Documented in spec.
bool canYieldVoid(zvalue node) {
    switch (nodeRecType(node)) {
        case NODE_apply: { return true;  }
        case NODE_call:  { return true;  }
        case NODE_fetch: { return true;  }
        case NODE_maybe: { return true;  }
        case NODE_store: { return true;  }
        case NODE_void:  { return true;  }
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
        if (cmpEqNullOk(repeat, SYM(CH_STAR))
            || cmpEqNullOk(repeat, SYM(CH_PLUS))) {
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
        if (!(cmpEqNullOk(repeat, SYM(CH_QMARK))
              || cmpEqNullOk(repeat, SYM(CH_STAR)))) {
            minArgs++;
        }
    }

    return intFromZint(minArgs);
}

// Documented in spec.
bool isExpression(zvalue node) {
    switch (nodeRecType(node)) {
        case NODE_apply:   { return true;  }
        case NODE_call:    { return true;  }
        case NODE_closure: { return true;  }
        case NODE_fetch:   { return true;  }
        case NODE_literal: { return true;  }
        case NODE_noYield: { return true;  }
        case NODE_store:   { return true;  }
        case NODE_varRef:  { return true;  }
        default:           { return false; }
    }
}
