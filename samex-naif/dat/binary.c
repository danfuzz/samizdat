// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <dlfcn.h>

#include "type/String.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * This is the type of `eval` functions defined by native code libraries.
 */
typedef zvalue evalFunction(zvalue);

/**
 * Loads the given file as a native dynamic library. Returns the
 * library handle (pointer) on success. Terminates the runtime with an
 * error message on failure.
 */
static void *loadLibrary(const char *path) {
    // RTLD_NOW: Perform all dynamic linking immediately, so if there are
    // undefined symbols, there will be an error here and now. That is, this
    // will cause a bad library to fail to load at all, rather than get
    // confused somewhere down the line.
    //
    // RTLD_LOCAL: Do not add symbols exported from this library to the
    // global symbol namespace. The only way to get at bindings is by using
    // the handle for this library.

    void *libHandle = dlopen(path, RTLD_NOW | RTLD_LOCAL);

    if (libHandle == NULL) {
        die("Trouble loading library %s: %s", path, dlerror());
    }

    return libHandle;
}

/**
 * Runs the `eval` function defined by the given library, passing it the
 * given `env` argument. This function returns whatever the library's
 * `eval` returns. If `eval` is not found, this terminates the runtime with
 * an error message.
 */
static zvalue evalLibrary(void *libHandle, zvalue env) {
    // The circumlocution used to assign `evalFn` is required, because
    // directly assigning a `void *` to a function pointer type is
    // an undefined operation in ISO C. Reference:
    //     <http://pubs.opengroup.org/onlinepubs/009695399/functions/dlsym.html>
    evalFunction *evalFn;
    *(void **)(&evalFn) = dlsym(libHandle, "eval");

    if (evalFn == NULL) {
        die("Trouble looking up `eval`: %s", dlerror());
    }

    return evalFn(env);
}


//
// Exported Definitions
//

// Documented in header.
zvalue datEvalBinary(zvalue env, zvalue path) {
    zint size = utf8SizeFromString(path);
    char str[size + 1];
    utf8FromString(size + 1, str, path);

    if ((size == 0) || (str[0] != '/')) {
        die("Invalid path: %s", str);
    }

    void *libHandle = loadLibrary(str);
    return evalLibrary(libHandle, env);

    // Note: We intentionally do not `dlclose` the library, on the assumption
    // that whatever `evalLibrary` returns either directly or indirectly
    // references stuff in the library.
}
