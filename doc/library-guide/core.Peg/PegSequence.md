Samizdat Layer 0: Core Library
==============================

core.Peg :: PegSequence
-----------------------

An instance of this class is a parser rule which runs a sequence of given
other rules (in order). This rule is successful only when all the given rules
also succeed. When successful, this rule yields the value yielded by the
last of its given rules, and returns updated state that reflects the
parsing in sequence of all the rules.

Each rule is passed a list of in-scope parsing results, starting with the
first result from the "closest" enclosing main sequence.

This is equivalent to the syntactic form `{: ... (rule1 rule2 etc) ... :}`.

A variant constructor matches literal strings, providing an equivalent to
the syntactic form `{: "string" :}`.


<br><br>
### Class Method Definitions

#### `.new(rules*) -> :PegSequence`

Creates an instance of this class.

#### `.newString(string) -> :PegSequence`

Creates an instance of this class, which matches the given string
exactly, as a sequence of characters. `string` must be a string. The result of
successful parsing is an empty-payload token with `@string` as its name.

<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.

