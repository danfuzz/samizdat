Samizdat Language Guide
=======================

Appendix: Layer 1 Tree Grammar
------------------------------

The following is a nearly complete tree grammar for Samizdat Layer 1,
written in Samizdat Layer 1, with commentary calling out the parts
that are needed specifically for Layer 1. Anything left unmarked is
also needed for Layer 0. In addition, places where higher layers add
or vary rules are noted in comments.

A program is parsed by matching the `program` rule, which yields a
`closure` node. For simple error handling, the rule `programOrError`
can be used.

```
def Format     = moduleLoad(["core", "Format"]);
def Generator  = moduleLoad(["core", "Generator"]);
def Lang0      = moduleLoad(["core", "Lang0"]);
def Number     = moduleLoad(["proto", "Number"]);
def Peg        = moduleLoad(["core", "Peg"]);
def Range      = moduleLoad(["core", "Range"]);
def Sequence   = moduleLoad(["core", "Sequence"]);

def Lang0Node = moduleLoad(["core", "Lang0Node"]);
def REFS               = Lang0Node::REFS;
def get_formals        = Lang0Node::get_formals;
def get_interpolate    = Lang0Node::get_interpolate;
def get_name           = Lang0Node::get_name;
def get_statements     = Lang0Node::get_statements;
def get_value          = Lang0Node::get_value;
def get_yieldDef       = Lang0Node::get_yieldDef;
def makeApply          = Lang0Node::makeApply;
def makeCall           = Lang0Node::makeCall;
def makeCallOrApply    = Lang0Node::makeCallOrApply;
def makeCallThunks     = Lang0Node::makeCallThunks;
def makeGet            = Lang0Node::makeGet;
def makeInterpolate    = Lang0Node::makeInterpolate;
def makeJump           = Lang0Node::makeJump;
def makeLiteral        = Lang0Node::makeLiteral;
def makeOptValue       = Lang0Node::makeOptValue;
def makeThunk          = Lang0Node::makeThunk;
def makeVarBind        = Lang0Node::makeVarBind;
def makeVarDef         = Lang0Node::makeVarDef;
def makeVarDefMutable  = Lang0Node::makeVarDefMutable;
def makeVarRef         = Lang0Node::makeVarRef;
def makeVarRefLvalue   = Lang0Node::makeVarRefLvalue;
def withoutInterpolate = Lang0Node::withoutInterpolate;


##
## Helper Definitions
##

## Set-like map of all lowercase identifier characters. Used to figure
## out if we're looking at a keyword in the `identifierString` rule.
def LOWER_ALPHA = {
    (Range::makeInclusiveRange("a", "z"))*: true
};


##
## Layer 0 Rules
##
## This section consists of the definitions required to implement Layer 0,
## with comments indicating the "hooks" for higher layers.
##

## Forward declarations required for layer 2. These are all add-ons to
## layer 0 or 1 rules, used to expand the syntactic possibilities of the
## indicated base forms.
def parExpression2;
def parPostfixOperator2;
def parPrefixOperator2;
def parStatement2;
def parTerm2;

## Forward declaration required for integrating layer 1 definitions.
def parParser;

## Forward declarations.
def parAssignExpression;
def parFnExpression;
def parOpExpression;
def parProgramBody;
def parUnaryExpression;

## Parses an expression in general.
def parExpression = {:
    ## This one's the top level "regular-looking" expression (in that it
    ## covers the territory of C-style expressions).
    %parAssignExpression
|
    ## This one is only nominally "regular-looking" (in that not many C
    ## family languages have function expressions).
    %parFnExpression
|
    ## Note: Layer 2 adds additional rules here.
    %parExpression2
:};

## Parses a parenthesized expression. This produces a result identical to
## the inner `expression` node, except without an `interpolate` binding.
def parParenExpression = {:
    @"("
    ex = parExpression

    (
        ## Reject commas explicitly, to make for a better error message and
        ## also avoid letting a would-be parenthesized expression turn out to
        ## be taken to be a function application argument list.
        @","
        { die("Comma not allowed within parenthesized expression.") }
    )?

    @")"

    { <> withoutInterpolate(ex) }
:};

## Parses a variable reference.
def parVarRef = {:
    name = @identifier
    { <> makeVarRefLvalue(dataOf(name)) }
:};

## Parses a variable definition or declaration.
def parVarDef = {:
    style = [@def @var]
    name = @identifier
    optExpr = (@"=" parExpression)?

    {
        def nameString = dataOf(name);
        <> ifIs { <> hasType(style, @@def) }
            { <> makeVarDef(nameString, optExpr*) }
            { <> makeVarDefMutable(nameString, optExpr*) }
    }
:};

## Parses a yield / nonlocal exit definition, yielding the def name.
def parYieldDef = {:
    @"<"
    name = @identifier
    @">"
    { <> dataOf(name) }
:};

## Parses an optional yield / nonlocal exit definition, always yielding
## a map (an empty map if no yield def was present).
def parOptYieldDef = {:
    y = parYieldDef
    { <> {yieldDef: y} }
|
    { <> {} }
:};

## Parses a formal argument decalaration.
def parFormal = {:
    name = (
        n = @identifier
        { <> {name: dataOf(n)} }
    |
        @"." { <> {} }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { <> {repeat: get_typeName(r)} }
    |
        { <> {} }
    )

    { <> {name*, repeat*} }
:};

## Parses a list of formal arguments, with no surrounding parentheses.
def parFormalsList = {:
    one = parFormal
    rest = (@"," parFormal)*
    { <> [one, rest*] }
|
    { <> [] }
:};

## Parses program / function declarations.
def parProgramDeclarations = {:
    yieldDef = parOptYieldDef
    formals = parFormalsList

    (@"->" | &@"<>")

    { <> {formals, yieldDef*} }
|
    { <> {formals: []} }
:};

## Parses a program (top-level program or contents inside function braces).
def parProgram = {:
    decls = parProgramDeclarations
    body = %parProgramBody
    { <> @closure{decls*, body*} }
:};

## Parses a closure (in-line anonymous function, with no extra bindings).
def parClosure = {:
    @"{"
    prog = parProgram
    @"}"
    { <> prog }
:};

## Parses a closure which must not define any formal arguments. This is done
## by parsing an arbitrary closure and then verifying that it does not
## declare formals. This is preferable to not-including formal argument
## syntax, because (a) no rule wants to differentiate these cases (rules
## either want an arbitrary closure or a specifically-constrained kind); (b)
## it reduces redundancy in the syntax, and (c) the error case on the former
## would be more obscure (as in just something like "unexpected token" on
## the would-be formal argument).
def parNullaryClosure = {:
    c = parClosure

    {
        def formals = get_formals(c);
        ifIs { <> ne(formals, []) }
            { die("Invalid formal argument in code block.") };
        <> c
    }
:};

## Parses a closure which must have neither formal arguments nor a yield
## definition. See `parseNullaryClosure` above for discussion.
def parCodeOnlyClosure = {:
    c = parNullaryClosure

    {
        ifIs { <> get_yieldDef(c) }
            { die("Invalid yield definition in code block.") };
        <> c
    }
:};

## Common parsing for `fn` statements and expressions. The syntax for
## both is identical, except that the statement form requires that the
## function be named. The result of this rule is a map identical in form to
## what's required for a closure payload, except that `name` may also
## be bound.
##
## The result of this rule is a `closure` node, with `name` possibly
## (but not necssarily) bound in the payload.
##
## The translation is along these lines:
##
## ```
## fn <out> name(arg1, arg2) { stat1; stat2 }
## ```
## =>
## ```
## { <\"return"> arg1, arg2 ->
##     def out = \"return";
##     stat1;
##     stat2
## }
## ```
##
## with:
##
## * no yield def binding statement if an explicit yield def was not present.
##
## * the key `name` bound to the function name, if a name was defined.
def parFnCommon = {:
    @fn

    ## This is a variable definition statement which binds the yield def
    ## name to the `return` function, if there is in fact a yield def present.
    returnDef = (
        y = parYieldDef
        { <> makeVarDef(y, REFS::return) }
    )?

    name = (
        n = @identifier
        { <> {name: dataOf(n)} }
    |
        { <> {} }
    )

    @"("
    formals = parFormalsList
    @")"

    code = parCodeOnlyClosure

    {
        def statements = [returnDef*, get_statements(code)*];
        <> @closure{
            dataOf(code)*,
            name*,
            formals,
            yieldDef: "return",
            statements
        }
    }
:};

## Parses a `fn` definition statement. The syntax here is the same as
## what's recognized by `parFnCommon`, except that the name is required.
## We don't error out (terminate the runtime) on a missing name, though, as
## that just means that we're looking at a legit `fn` expression, which will
## get successfully parsed by the `expression` alternative of `statement`.
def parFnDef = {:
    closure = parFnCommon

    name = { <> get_name(closure) }
    {
        ## `@topDeclaration` is split apart in the `programBody` rule.
        <> @topDeclaration{
            top:  makeVarDef(name),
            main: makeVarBind(name, closure)
        }
    }
:};

## Parses a `fn` (function with `return` binding) expression. The translation
## is as described in `parFnCommon` (above) if the function is not given a
## name. If the function *is* given a name, the translation is along the
## following lines (so as to enable self-recursion):
##
## ```
## fn <out> name ...
## ```
## =>
## ```
## {
##     def name;
##     <> name := { <out> ... }
## }()
## ```
parFnExpression := {:
    closure = parFnCommon

    (
        name = { <> get_name(closure) }
        {
            def mainClosure = @closure{
                formals:    [],
                statements: [makeVarDef(name)],
                yield:      makeVarBind(name, closure)
            };

            <> makeCall(mainClosure)
        }
    |
        { <> closure }
    )
:};

## Parses an integer literal. Note: This includes parsing a `-` prefix,
## so that simple negative constants aren't turned into complicated function
## calls.
def parInt = {:
    @"-"
    i = @int
    { <> makeLiteral(Number::neg(dataOf(i))) }
|
    i = @int
    { <> makeLiteral(dataOf(i)) }
:};

## Parses a string literal.
def parString = {:
    s = @string
    { <> makeLiteral(dataOf(s)) }
:};

## Parses an identifier, identifier-like keyword, or string literal,
## returning a string literal in all cases.
def parIdentifierString = {:
    parString
|
    ident = @identifier
    { <> makeLiteral(dataOf(ident)) }
|
    token = .
    {
        <> ifNot { <> dataOf(token) }
            {
                def type = get_typeName(token);
                def firstCh = nth(type, 0);
                <> ifIs { <> get(LOWER_ALPHA, firstCh) }
                    { <> makeLiteral(type) }
            }
    }
:};

## Parses a map key.
def parKey = {:
    key = parIdentifierString
    @":"
    { <> key }
|
    key = parExpression
    @":"
    { <> key }
:};

## Parses a mapping (element of a map).
def parMapping = {:
    keys = parKey*
    value = parExpression

    {
        <> ifIs { <> eq(keys, []) }
            { <out> ->
                ## No keys were specified, so the value must be either a
                ## whole-map interpolation or a variable-name-to-its-value
                ## binding.
                ifValue { <> get_interpolate(value) }
                    { interp -> <out> interp };
                ifIs { <> hasType(value, @@varRef) }
                    {
                        <out> makeCall(REFS::makeValueMap,
                            makeLiteral(get_name(value)), value)
                    }
            }
            {
                ## One or more keys.
                <> makeCallOrApply(REFS::makeValueMap,
                    keys*, withoutInterpolate(value))
            }
    }
:};

## Parses a map literal.
def parMap = {:
    @"{"

    result = (
        one = parMapping
        rest = (@"," parMapping)*
        {
            <> ifIs { <> eq(rest, []) }
                { <> one }
                { <> makeCall(REFS::cat, one, rest*) }
        }
    |
        { <> makeLiteral({}) }
    )

    @"}"

    { <> result }
:};

## Parses a list item or function call argument. This handles all of:
##
## * accepting general expressions
## * rejecting expressions that look like `key:value` mappings. This is
##   effectively "reserved syntax" (for future expansion); rejecting this
##   here means that `x:y` won't be mistaken for other valid syntax.
def parListItem = {:
    parIdentifierString
    @":"
    { die("Mapping syntax not valid as a list item or call argument.") }
|
    parExpression
:};

## Parses an "unadorned" (no bracketing) list. Yields a list (per se)
## of contents.
def parUnadornedList = {:
    one = parListItem
    rest = (@"," parListItem)*
    { <> [one, rest*] }
|
    { <> [] }
:};

## Parses a list literal.
def parList = {:
    @"["
    expressions = parUnadornedList
    @"]"
    {
        <> ifIs { <> eq(expressions, []) }
            { <> makeLiteral([]) }
            { <> makeCallOrApply(REFS::makeList, expressions*) }
    }
:};

## Parses a type name, yielding a type value. If the name is a blatant
## literal, then the result of this rule is also a literal. If not, the
## result of this rule is a call to `makeDerivedDataType`.
def parTypeName = {:
    name = (parIdentifierString | parParenExpression)

    {
        <> ifIs { <> hasType(name, @@literal) }
            { <> makeLiteral(@@(get_value(name))) }
            { <> makeCall(REFS::makeDerivedDataType, name) }
    }
:};

## Parses a type literal form.
def parType = {:
    @"@@"
    parTypeName
:};

## Parses a literal in derived value form.
def parDeriv = {:
    @"@"

    type = parTypeName
    value = (parParenExpression | parMap | parList)?

    { <> makeCall(REFS::makeValue, type, value*) }
:};

## Parses a term (basic expression unit). **Note:** Parsing for `Map` needs
## to be done before `List`, since the latter rejects "map-like" syntax as a
## fatal error.
def parTerm = {:
    parVarRef | parInt | parString | parMap | parList |
    parDeriv | parType | parClosure | parParenExpression
|
    ## Defined by Samizdat Layer 1. The lookahead is just to make it clear
    ## that Layer 1 can only be "activated" with that one specific token.
    &@"{:" %parParser
|
    ## Defined by Samizdat Layer 2.
    &[@interpolatedString @"(" @"["] %parTerm2
:};

## Parses a list of "actual" (as opposed to formal) arguments to a function.
## Yields a list of expression nodes.
def parActualsList = {:
    @"("
    normalActuals = parUnadornedList
    @")"
    closureActuals = parClosure*
    { <> [closureActuals*, normalActuals*] }
|
    parClosure+
:};

## Parses a unary postfix operator. This yields a function (per se) to call
## in order to construct a node that represents the appropriate ultimate
## function call.
def parPostfixOperator = {:
    actuals = parActualsList
    { <> { node <> makeCallOrApply(node, actuals*) } }
|
    ## This is sorta-kinda a binary operator, but in terms of precedence it
    ## fits better here.
    @"::"
    key = parIdentifierString
    { <> { node <> makeGet(node, key) } }
|
    ## The lookahead failure here is to make the grammar prefer `*` to be
    ## treated as a binary op. (`*` is only defined as postfix in Layer 0,
    ## but higher layers augment its meaning.)
    @"*" !parExpression
    { <> { node <> makeInterpolate(node) } }
|
    @"?"
    { <> { node <> makeOptValue(node) } }
|
    ## Note: Layer 2 adds additional rules here.
    %parPostfixOperator2
:};

## Parses a unary expression. This is a term, optionally surrounded on
## either side by any number of unary operators. Postfix operators
## take precedence over (are applied before) the prefix operators.
parUnaryExpression := {:
    ## The rule is written this way in order to ensure that the `-`
    ## in front of a numeric constant gets parsed as a term and not as
    ## a unary expression.
    basePrefixes = (
        base = parTerm
        { <> {base, prefixes: []} }
    |
        ## Note: Layer 2 adds prefix operator parsing here.
        prefixes = (%parPrefixOperator2)*
        base = parTerm
        ## Reverse the `prefixes` list, so that prefixes are applied
        ## in outward order from the base term.
        { <> {base, prefixes: Sequence::reverse(prefixes)} }
    )

    postfixes = parPostfixOperator*

    {
        def prefixes = basePrefixes::prefixes;
        var result = basePrefixes::base;

        Generator::filterPump(postfixes) { op -> result := op(result) };
        Generator::filterPump(prefixes) { op -> result := op(result) };
        <> result
    }
:};

## Parses an operator-bearing expression (or simple term). This is a trivial
## passthrough to `unaryExpression` in layer 0, but is expanded significantly
## in layer 2.
parOpExpression := parUnaryExpression;

## Parses an assignment expression, or passes through to parse a regular
## `opExpression`. An lvalue is parsed here by first parsing an arbitrary
## `opExpression` and then extracting the `lvalue` constructor out of it.
## This fails (gracefully) if there is no `lvalue` to extract from a given
## expression.
parAssignExpression := {:
    base = %parOpExpression

    (
        @":="
        lvalue = { <> get_lvalue(base) }
        ex = parExpression
        { <> lvalue(ex) }
    |
        { <> base }
    )
:};

## Note: There are additional expression rules in Layer 2 and beyond.
def parStatement = {:
    parVarDef | parFnDef | parExpression
|
    ## Note: Layer 2 adds additional rules here.
    %parStatement2
:};

## Parses a nonlocal exit / return. All of the forms matched by this rule
## have the dual properties of (a) necessarily being at the end of a code
## block, and (b) being represented as a `jump` call in the underlying
## tree representation.
def parNonlocalExit = {:
    name = (
        @"<"
        n = parVarRef
        @">"
        { <> n }
    |
        op = [@break @continue @return]
        { <> makeVarRef(get_typeName(op)) }
    )

    optValue = parExpression?

    { <> makeJump(name, optValue*) }
:};

## Parses a local yield / return.
def parYield = {:
    @"<>"
    (
        ex = parExpression
        { <> {yield: ex} }
    |
        { <> {} }
    )
:};

## Parses a program body (statements plus optional yield).
parProgramBody := {:
    @";"*

    most = (
        s = parStatement
        @";"+
        { <> s }
    )*

    last = (
        s = (parStatement | parNonlocalExit)
        { <> {statements: [s]} }
    |
        y = parYield
        { <> {statements: [], y*} }
    |
        { <> {statements: []} }
    )

    @";"*

    {
        def rawStatements = [most*, last::statements*];
        def tops = Generator::filterAll(rawStatements)
            { s ->
                <> ifIs { <> hasType(s, @@topDeclaration) }
                    { <> s::top }
            };
        def mains = Generator::filterAll(rawStatements)
            { s ->
                <> ifIs { <> hasType(s, @@topDeclaration) }
                    { <> s::main }
                    { <> s }
            };
        <> {last*, statements: [tops*, mains*]}
    }
:};

## Top-level rule to parse an expression with possible error afterwards.
def parExpressionOrError = {:
    ex = parExpression
    (
        pending = .+
        { reportError(pending) }
    )?
    { <> ex }
:};

## Top-level rule to parse a program with possible error afterwards.
## Note that an empty string is a valid program.
def parProgramOrError = {:
    prog = parProgram
    (
        pending = .+
        { reportError(pending) }
    )?
    { <> prog }
:};


##
## Layer 1 Rules
##
## This section consists of the definitions required to implement Layer 1,
## above and beyond the preceding section.
##
## **Note:** The grammar uses the label "pex" to denote various
## "Parser EXpression" types.
##

## Forward declaration.
def parChoicePex;

## Map from parser token types to derived value types for pexes.
def PEX_TYPES = {
    @@"&": "lookaheadSuccess",
    @@"!": "lookaheadFailure",
    @@"?": "opt",
    @@"*": "star",
    @@"+": "plus"
};

## Parses a parser function.
parParser := {:
    @"{:"
    pex = %parChoicePex
    @":}"
    { <> @parser{value: pex} }
:};

## Parses a parenthesized parsing expression.
def parParenPex = {:
    @"("
    pex = %parChoicePex
    @")"
    { <> pex }
:};

## Parses a string literal parsing expression.
def parParserString = {:
    @string
:};

## Parses a token literal parsing expression.
def parParserToken = {:
    @"@"
    type = parIdentifierString
    { <> @token(@@(get_value(type))) }
:};

## Parses a string or character range parsing expression, used when defining
## sets. Yields a string per se (not a token).
def parParserSetString = {:
    s = @string

    (
        @".."
        end = @string

        { <out> ->
            def startChar = dataOf(s);
            def endChar = dataOf(end);

            ## Reject non-single-character strings.
            ifIs { <> ne(1, sizeOf(startChar)) } { <out> };
            ifIs { <> ne(1, sizeOf(endChar)) } { <out> };

            <> cat(Range::makeInclusiveRange(startChar, endChar)*)
        }
    |
        { <> dataOf(s) }
    )
:};

## Parses a set (or set complement) parsing expression.
def parParserSet = {:
    @"["

    type = (
        @"!" { <> "tokenSetComplement" }
    |
        { <> "tokenSet" }
    )

    terminals = (
        strings = parParserSetString+
        { <> collect(cat(strings*), { ch <> @@(ch) }) }
    |
        tokens = parParserToken+
        { <> collect(tokens, dataOf) }
    |
        { <> [] }
    )

    @"]"

    { <> @(type)(terminals) }
:};

## Parses a code block parsing expression.
def parParserCode = {:
    closure = parNullaryClosure
    { <> @code(dataOf(closure)) }
:};

## Parses a thunk parsing expression.
def parParserThunk = {:
    @"%"
    term = parTerm
    { <> @thunk(term) }
:};

## Parses a parsing expression term.
def parParserTerm = {:
    @"."
    { <> @any }
|
    @"("
    @")"
    { <> @empty }
|
    parVarRef | parParserString | parParserToken | parParserSet |
    parParserCode | parParserThunk | parParenPex
:};

## Parses a repeat (or not) parsing expression.
def parRepeatPex = {:
    term = parParserTerm
    (
        repeat = [@"?" @"*" @"+"]
        { <> @(get(PEX_TYPES, get_type(repeat)))(term) }
    |
        { <> term }
    )
:};

## Parses a lookahead (or not) parsing expression. This covers both lookahead
## success and lookahead failure.
def parLookaheadPex = {:
    (
        lookahead = [@"&" @"!"]
        pex = parRepeatPex
        { <> @(get(PEX_TYPES, get_type(lookahead)))(pex) }
    )
|
    parRepeatPex
:};

## Parses a name (or not) parsing expression.
def parNamePex = {:
    (
        name = @identifier
        @"="
        pex = parLookaheadPex
        { <> @varDef{name: dataOf(name), value: pex} }
    )
|
    parLookaheadPex
:};

## Parses a sequence parsing expression. This includes sequences of length
## one, but it does *not* parse empty (zero-length) sequences.
def parSequencePex = {:
    items = parNamePex+
    { <> @sequence(items) }
:};

## Parses a choice parsing expression. This includes a single choice.
parChoicePex := {:
    one = parSequencePex
    rest = (@"|" parSequencePex)*
    { <> @choice[one, rest*] }
:};


##
## Layer 2 Rule Stubs
##

## In layer 2, these are all non-trivial, but here they are simply
## always-fail.

parExpression2      := {: !() :};
parPostfixOperator2 := {: !() :};
parPrefixOperator2  := {: !() :};
parStatement2       := {: !() :};
parTerm2            := {: !() :};
```
