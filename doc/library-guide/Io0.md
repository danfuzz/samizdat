Samizdat Layer 0: Core Library
==============================

core::Io0
---------

This module defines primitive I/O operations. It is only meant to be used
by the lowest layer of core library.


<br><br>
### Generic Function Definitions

(none)


<br><br>
### Function Definitions

#### `die(string?) <> n/a # Terminates the runtime.`

This is the same as the same-named function in the `core::Io1` module.

#### `flatCwd() <> flatPath`

Returns the current working directory of the process, as a
string.

This function is a thin veneer over the standard Posix call `getcwd()`.

#### `flatReadLink(flatPath) <> flatPath | void`

Checks the filesystem to see if the given path (given as a flat string)
refers to a symbolic link. If it does, then this returns the string which
represents the direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

This function is a thin veneer over the standard Posix call `readlink()`.

#### `flatFileExists(flatPath) <> logic`

Returns `flatPath` if it corresponds to an already-existing file.
Returns void if not.

#### `flatReadFileUtf8(flatPath) <> string`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a string
of the read and decoded text.

#### `flatWriteFileUtf8(flatPath, text) <> void`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a string) as a stream of UTF-8 bytes.

#### `note(string) <> void`

This is the same as the same-named function in the `core::Io1` module.
