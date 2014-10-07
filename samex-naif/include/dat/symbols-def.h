// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Symbol definitions needed by core classes.
//
// **Note:** This file gets `#include`d multiple times, and so does not
// have the usual guard macros.
//
// `DEF_SYMBOL(name)` -- Defines an interned symbol.
//

// The following are all method names. See the spec for details.
DEF_SYMBOL(call);
DEF_SYMBOL(cat);
DEF_SYMBOL(debugString);
DEF_SYMBOL(debugSymbol);
DEF_SYMBOL(del);
DEF_SYMBOL(get);
DEF_SYMBOL(get_name);
DEF_SYMBOL(get_size);
DEF_SYMBOL(perEq);
DEF_SYMBOL(perOrder);
DEF_SYMBOL(put);
DEF_SYMBOL(toString);
DEF_SYMBOL(totalEq);
DEF_SYMBOL(totalOrder);
DEF_SYMBOL(and);
DEF_SYMBOL(bit);
DEF_SYMBOL(bitSize);
DEF_SYMBOL(not);
DEF_SYMBOL(or);
DEF_SYMBOL(shl);
DEF_SYMBOL(shr);
DEF_SYMBOL(xor);
DEF_SYMBOL(abs);
DEF_SYMBOL(add);
DEF_SYMBOL(div);
DEF_SYMBOL(divEu);
DEF_SYMBOL(mod);
DEF_SYMBOL(modEu);
DEF_SYMBOL(mul);
DEF_SYMBOL(neg);
DEF_SYMBOL(sign);
DEF_SYMBOL(sub);
DEF_SYMBOL(store);
DEF_SYMBOL(keyList);
DEF_SYMBOL(nthMapping);
DEF_SYMBOL(valueList);
DEF_SYMBOL(get_parent);
DEF_SYMBOL(collect);
DEF_SYMBOL(fetch);
DEF_SYMBOL(nextValue);


/**
 * Method `.gcMark()`: Does GC marking for the given value.
 *
 * TODO: This should be defined as an unlisted symbol and *not* exported
 * in any way to the higher layer environment.
 */
DEF_SYMBOL(gcMark);

/** Used as a key when accessing modules. */
DEF_SYMBOL(exports);

/** Used as a key when accessing modules. */
DEF_SYMBOL(imports);

/** Used as a key when accessing modules. */
DEF_SYMBOL(main);

/** Used as the prefix for metaclass names. */
DEF_SYMBOL(meta_);

/** Used as a key when accessing modules. */
DEF_SYMBOL(resources);
