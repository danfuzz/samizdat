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

// The following are all class names. See the spec for details.
DEF_SYMBOL(Bool);
DEF_SYMBOL(Box);
DEF_SYMBOL(Builtin);
DEF_SYMBOL(Class);
DEF_SYMBOL(Core);
DEF_SYMBOL(Int);
DEF_SYMBOL(Jump);
DEF_SYMBOL(List);
DEF_SYMBOL(Map);
DEF_SYMBOL(MapCache);
DEF_SYMBOL(Metaclass);
DEF_SYMBOL(Null);
DEF_SYMBOL(Object);
DEF_SYMBOL(Record);
DEF_SYMBOL(String);
DEF_SYMBOL(Symbol);
DEF_SYMBOL(SymbolTable);
DEF_SYMBOL(Value);

// The following are all method names. See the spec for details.
DEF_SYMBOL(abs);
DEF_SYMBOL(accepts);
DEF_SYMBOL(add);
DEF_SYMBOL(and);
DEF_SYMBOL(bit);
DEF_SYMBOL(bitSize);
DEF_SYMBOL(call);
DEF_SYMBOL(castFrom);
DEF_SYMBOL(castToward);
DEF_SYMBOL(cat);
DEF_SYMBOL(collect);
DEF_SYMBOL(debugString);
DEF_SYMBOL(debugSymbol);
DEF_SYMBOL(del);
DEF_SYMBOL(div);
DEF_SYMBOL(divEu);
DEF_SYMBOL(fetch);
DEF_SYMBOL(get);
DEF_SYMBOL(get_data);
DEF_SYMBOL(get_key);
DEF_SYMBOL(get_name);
DEF_SYMBOL(get_parent);
DEF_SYMBOL(get_size);
DEF_SYMBOL(get_value);
DEF_SYMBOL(hasName);
DEF_SYMBOL(isInterned);
DEF_SYMBOL(keyList);
DEF_SYMBOL(mod);
DEF_SYMBOL(modEu);
DEF_SYMBOL(mul);
DEF_SYMBOL(neg);
DEF_SYMBOL(nextValue);
DEF_SYMBOL(not);
DEF_SYMBOL(nth);
DEF_SYMBOL(nthMapping);
DEF_SYMBOL(or);
DEF_SYMBOL(perEq);
DEF_SYMBOL(perOrder);
DEF_SYMBOL(put);
DEF_SYMBOL(readResource);
DEF_SYMBOL(resolve);
DEF_SYMBOL(reverse);
DEF_SYMBOL(shl);
DEF_SYMBOL(shr);
DEF_SYMBOL(sign);
DEF_SYMBOL(sliceExclusive);
DEF_SYMBOL(sliceInclusive);
DEF_SYMBOL(store);
DEF_SYMBOL(sub);
DEF_SYMBOL(toString);
DEF_SYMBOL(toUnlisted);
DEF_SYMBOL(totalEq);
DEF_SYMBOL(totalOrder);
DEF_SYMBOL(valueList);
DEF_SYMBOL(xor);


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
