/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Execution frames
 */

#include "impl.h"
#include "type/Map.h"
#include "type/Value.h"
#include "util.h"


/*
 * Module Definitions
 */

/* Documented in header. */
void frameInit(Frame *frame, Frame *parentFrame, zvalue parentClosure,
        zvalue vars) {
    pbAssertValidOrNull(parentClosure);
    pbAssertValid(vars);

    if ((parentFrame != NULL) && !parentFrame->onHeap) {
        die("Stack-allocated `parentFrame`.");
    }

    frame->parentFrame = parentFrame;
    frame->parentClosure = parentClosure;
    frame->vars = vars;
    frame->onHeap = false;
}

/* Documented in header. */
void frameMark(Frame *frame) {
    pbMark(frame->vars);
    pbMark(frame->parentClosure); // This will mark `parentFrame`.
}

/* Documented in header. */
void frameAdd(Frame *frame, zvalue name, zvalue value) {
    zvalue vars = frame->vars;
    zvalue newVars = mapPut(vars, name, value);

    if (pbSize(vars) == pbSize(newVars)) {
        die("Variable already defined: %s", pbDebugString(name));
    }

    frame->vars = newVars;
}

/* Documented in header. */
zvalue frameGet(Frame *frame, zvalue name) {
    for (/*frame*/; frame != NULL; frame = frame->parentFrame) {
        zvalue result = mapGet(frame->vars, name);

        if (result != NULL) {
            return result;
        }
    }

    die("Variable not defined: %s", pbDebugString(name));
}

/* Documented in header. */
void frameSnap(Frame *target, Frame *source) {
    pbAssertValidOrNull(source->parentClosure);
    pbAssertValid(source->vars);

    *target = *source;
    target->onHeap = true;
}
