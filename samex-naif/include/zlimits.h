// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// A grab-bag of various implementation limits / limitations and
// generally tweakable implementation constants. The prefix on each
// value's name indicates which module "owns" it.
//

#ifndef _ZLIMITS_H_
#define _ZLIMITS_H_

enum {
    /** Maximum readable file size, in bytes. */
    IO_MAX_FILE_SIZE = 100000,

    /** Maximum number of formal arguments to a function. */
    LANG_MAX_FORMALS = 10,

    /**
     * Maximum number of tokens in a given parse (which is the maximum
     * number of characters for a tokenizer).
     */
    LANG_MAX_TOKENS = 100000,

    /**
     * Maximum number of characters in a tokenized string constant,
     * identifier, or directive.
     */
    LANG_MAX_STRING_CHARS = 200
};

#endif
