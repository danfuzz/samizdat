// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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
DEF_SYMBOL(Cell);
DEF_SYMBOL(Class);
DEF_SYMBOL(Closure);
DEF_SYMBOL(ClosureNode);
DEF_SYMBOL(Cmp);
DEF_SYMBOL(Core);
DEF_SYMBOL(ExecNode);
DEF_SYMBOL(If);
DEF_SYMBOL(Int);
DEF_SYMBOL(Jump);
DEF_SYMBOL(Lazy);
DEF_SYMBOL(List);
DEF_SYMBOL(Map);
DEF_SYMBOL(Metaclass);
DEF_SYMBOL(Null);
DEF_SYMBOL(NullBox);
DEF_SYMBOL(Object);
DEF_SYMBOL(Promise);
DEF_SYMBOL(Record);
DEF_SYMBOL(Result);
DEF_SYMBOL(String);
DEF_SYMBOL(Symbol);
DEF_SYMBOL(SymbolTable);
DEF_SYMBOL(Value);

// The following are all method names. See the spec for details.
DEF_SYMBOL(abs);
DEF_SYMBOL(accepts);
DEF_SYMBOL(add);
DEF_SYMBOL(and);
DEF_SYMBOL(andThenElse);
DEF_SYMBOL(bit);
DEF_SYMBOL(bitSize);
DEF_SYMBOL(call);
DEF_SYMBOL(cases);
DEF_SYMBOL(castFrom);
DEF_SYMBOL(castToward);
DEF_SYMBOL(cat);
DEF_SYMBOL(collect);
DEF_SYMBOL(crossEq);
DEF_SYMBOL(crossOrder);
DEF_SYMBOL(debugString);
DEF_SYMBOL(debugSymbol);
DEF_SYMBOL(del);
DEF_SYMBOL(div);
DEF_SYMBOL(divEu);
DEF_SYMBOL(eq);
DEF_SYMBOL(fetch);
DEF_SYMBOL(forEach);
DEF_SYMBOL(fromLogic);
DEF_SYMBOL(ge);
DEF_SYMBOL(get);
DEF_SYMBOL(get_data);
DEF_SYMBOL(get_key);
DEF_SYMBOL(get_name);
DEF_SYMBOL(get_parent);
DEF_SYMBOL(get_size);
DEF_SYMBOL(get_value);
DEF_SYMBOL(gt);
DEF_SYMBOL(hasName);
DEF_SYMBOL(is);
DEF_SYMBOL(isInterned);
DEF_SYMBOL(keyList);
DEF_SYMBOL(le);
DEF_SYMBOL(loop);
DEF_SYMBOL(loopUntil);
DEF_SYMBOL(lt);
DEF_SYMBOL(maybeValue);
DEF_SYMBOL(mod);
DEF_SYMBOL(modEu);
DEF_SYMBOL(mul);
DEF_SYMBOL(ne);
DEF_SYMBOL(neg);
DEF_SYMBOL(new);
DEF_SYMBOL(nextValue);
DEF_SYMBOL(not);
DEF_SYMBOL(nth);
DEF_SYMBOL(of);
DEF_SYMBOL(or);
DEF_SYMBOL(order);
DEF_SYMBOL(perEq);
DEF_SYMBOL(perGe);
DEF_SYMBOL(perGt);
DEF_SYMBOL(perLe);
DEF_SYMBOL(perLt);
DEF_SYMBOL(perNe);
DEF_SYMBOL(perOrder);
DEF_SYMBOL(readResource);
DEF_SYMBOL(repeat);
DEF_SYMBOL(resolve);
DEF_SYMBOL(reverse);
DEF_SYMBOL(reverseNth);
DEF_SYMBOL(shl);
DEF_SYMBOL(shr);
DEF_SYMBOL(sign);
DEF_SYMBOL(singleValue);
DEF_SYMBOL(sliceExclusive);
DEF_SYMBOL(sliceGeneral);
DEF_SYMBOL(sliceInclusive);
DEF_SYMBOL(store);
DEF_SYMBOL(sub);
DEF_SYMBOL(subclass);
DEF_SYMBOL(toLogic);
DEF_SYMBOL(toUnlisted);
DEF_SYMBOL(typeAccepts);
DEF_SYMBOL(typeCast);
DEF_SYMBOL(value);
DEF_SYMBOL(valueList);
DEF_SYMBOL(xor);


/** Used as a key for class configuration in `.subclass()`. */
DEF_SYMBOL(access);

/** Used as an argument to `.sliceGeneral()`. */
DEF_SYMBOL(exclusive);

/** Used as a key when accessing modules. */
DEF_SYMBOL(exports);

/** Used as a record tag in arguments to `.sliceGeneral()`. */
DEF_SYMBOL(fromEnd);

/** Used as a record tag in arguments to `.sliceGeneral()`. */
DEF_SYMBOL(fromStart);

/**
 * Method `.gcMark()`: Does GC marking for the given value.
 *
 * TODO: This should be defined as an unlisted symbol and *not* exported
 * in any way to the higher layer environment.
 */
DEF_SYMBOL(gcMark);

/** Used as a key when accessing modules. */
DEF_SYMBOL(imports);

/** Used as an argument to `.sliceGeneral()`. */
DEF_SYMBOL(inclusive);

/** Used as an `*order()` result. */
DEF_SYMBOL(less);

/** Used as a key when accessing modules. */
DEF_SYMBOL(main);

/** Used as the prefix for metaclass names. */
DEF_SYMBOL(meta_);

/** Used as an `*order()` result. */
DEF_SYMBOL(more);

/** Used as a key when accessing modules. */
DEF_SYMBOL(resources);

/** Used as an `*order()` result. */
DEF_SYMBOL(same);
