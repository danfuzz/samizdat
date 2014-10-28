Samizdat Layer 0: Core Library
==============================

core.Peg :: PegRepeat
---------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegRepeat`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeRepeat(rule, optMinSize?, optMaxSize?) -> rule`

Makes and returns a parser rule which matches another `rule` multiple
times, yielding a list of resulting matches. If `optMinSize*` is specified,
then this rule fails unless the size of the yielded list is at least as
given. If `optMaxSize*` is specified, then this rule will only ever match at
most that many repetitions.

These syntactic equivalences hold:

* `makeRepeat(rule)` is equivalent to `{: rule* :}`.
* `makeRepeat(rule, 1)` is equivalent to `{: rule+ :}`.
* `makeRepeat(rule, 0, 1)` is equivalent to `{: rule? :}`.
