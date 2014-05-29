Why C?
======

The bulk of the code for Samizdat that isn't self-hosted is written in C.
C was picked for a few reasons.

One goal of Samizdat is to produce normal system executables that can be run
when linked (static or dynamic linked) with a modest-sized system library.
This goal is incompatible with languages that require the use of an
interpreter-based runtime environment (even if the runtime includes
compilation), including in particular ones that use an executable format
that is different than the usual system executable format. For example,
Java is disqualified by the requirement to use the `.class` executable
file format. And JavaScript is disqualified by the requirement to use
source code as the executable format. There are of course ways to compile
these (and others) to native code; however, doing so would be unproductively
"fighting the tide" of these systems.

Samizdat defines a non-trivial object model / type system, and it aims
to be able to provide a minimally-layered implementation of that model.
As such, using another language with its own "strong" ideas about an
object model adds both cognitive load to the task of writing the system
as well as potential inefficiency. This is why, for example, C++ was
not picked.

Finally, Samizdat is meant to be a "language reboot" of sorts. Because of
this, it makes sense to minimize the amount of external dependencies on
the implementation.

The most prominent (if not only) well-established, current, and stable
language which fits all the above criteria is C. Building Samizdat from
source *only* requires a C compiler and linker. The code uses some extensions
to C which are available both with the Clang and Gnu C compilers, on
the theory that these extensions are available widely enough to be reasonably
used. As a practical matter, there are also shell scripts (written in Bash,
and using standard / widely available shell utilities) that are used to help
automate the build; however, these aren't strictly necessary. That is,
the shell stuff is a convenience that helps avoid what would be a lot of
compilation drudgery, but there's no real "implementation semantics" in the
scripts.
