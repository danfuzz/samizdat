Samizdat Language Guide
=======================

The Basics
----------

Samizdat is a functional-forward (but not purely functional) programming
language, meant to be familiar to those steeped in the traditions of the
[C programming language](https://en.wikipedia.org/wiki/C_%28programming_language%29)
family, including C per se, [C++](https://en.wikipedia.org/wiki/C%2B%2B),
[Java](https://en.wikipedia.org/wiki/Java_%28programming_language%29),
and [JavaScript](https://en.wikipedia.org/wiki/JavaScript).

The goal of Samizdat is to be a compelling language choice when building
applications that need to take into account the realities of computing
across address spaces over sometimes-high-latency communications links.

Samizdat is mostly an "expression language": Most syntactic constructs in
the language &mdash; including those which look like "statements" from the
C tradition &mdash; are in fact "expressions" that can be combined usefully
with any other expressions in the language.

### Comments

Samizdat has both single-line and multi-line comments.

A single-line comment starts with a number sign (`#`) and continues to the
end of the line it appears on.

```
#
# I am commentary.
thisIsNotCommentary   # ...but this *is* commentary.
```

A multi-line comment starts with the sequence slash-star (`/*`) and continues
to a matching star-slash (`*/`). Multi-line comments nest.

```
/* I am commentary. */ thisIsNotCommentary

/* This
is commentary. /* This too. */ Commentary
continues. */ thisIsNotCommentary
```
