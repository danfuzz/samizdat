// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Execution frames
//

#include "type/Box.h"
#include "type/String.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
#include "util.h"

#include "impl.h"


//
// Module Definitions
//

// Documented in header.
void frameInit(Frame *frame, Frame *parentFrame, zvalue parentClosure,
        zvalue vars) {
    if ((parentFrame != NULL) && !parentFrame->onHeap) {
        die("Stack-allocated `parentFrame`.");
    }

    frame->parentFrame = parentFrame;
    frame->parentClosure = parentClosure;
    frame->vars = vars;
    frame->onHeap = false;
}

// Documented in header.
void frameMark(Frame *frame) {
    datMark(frame->vars);
    datMark(frame->parentClosure);  // This will mark `parentFrame`.
}

// Documented in header.
void frameDef(Frame *frame, zvalue name, zvalue box) {
    frame->vars = symtabCatMapping(frame->vars, (zmapping) {name, box});
}

// Documented in header.
zvalue frameGet(Frame *frame, zvalue name) {
    for (/*frame*/; frame != NULL; frame = frame->parentFrame) {
        zvalue result = symtabGet(frame->vars, name);

        if (result != NULL) {
            return result;
        }
    }

    zvalue nameStr = cm_castFrom(CLS_String, name);
    die("Variable not defined: %s", cm_debugString(nameStr));
}

// Documented in header.
void frameSnap(Frame *target, Frame *source) {
    *target = *source;
    target->onHeap = true;
}
