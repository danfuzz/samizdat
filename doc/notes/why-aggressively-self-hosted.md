Why Aggressively Self-Hosted?
=============================

There are lots of language implementations in the world that are themselves
written mostly or entirely in other languages. Samizdat is not one of them.

Samizdat is instead "aggressively self-hosted," in that there is a small
amount of code written in another language (C) to provide a minimal runtime,
and everything else is written in the language itself. The self-hosted
part of Samizdat includes most of the core library, including the primary
parser for the language itself. The tool which compiles Samizdat to C
(for subsequent compilation with the system's C compiler) is also written
in Samizdat.

This arrangement adds some drag to the ongoing development of the
system. Any change to the language &mdash; its syntax, its semantics,
its library &mdash; has the potential to require both a direct change
(e.g. change the contract of a library function) as well as a "recursive"
change to the self-hosted portion of the system (e.g. fix the parser to use
that library function in its newly-required way).

The highest drag is from invasive syntactic changes (e.g. changing what a
function call looks like), because these require almost all the self-hosted
code to change.

The drag is unfortunate, but there is a compensatory benefit: The system
is its own practical use case. This is especially important for a project
which is (as of this writing) primarily a solo effort. Development energy
doesn't have to be overly devoted to building (and maintaining) toy demos,
nor does a ton of energy have to be put into theorizing how large projects
would work in the system. Instead, Samizdat is its own large project,
easily demonstrating its own deficiencies.

In the long run, many language implementations that start out as primarily
hosted atop other languages evolve into more purely self-hosted
implementations. Samizdat is merely aiming to be a bit ahead of the curve.
