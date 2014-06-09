Samizdat Language Guide
=======================

Appendix: Layer 1 Tree Grammar
------------------------------

The following is the complete tree grammar for Samizdat Layer 1,
written in Samizdat Layer 1, with commentary calling out the parts
that are needed specifically for Layer 1 as well as how Layer 2 hooks in.
Anything left unmarked is also needed for Layer 0.

A program is parsed by matching the `program` rule, which yields a
`closure` node. For simple error handling, the rule `programOrError`
can be used.

```
import core.Format;
import core.Generator;
import core.Lang0;
import core.Lang0Node :: *;
import core.Peg;
import core.Range;
import core.Sequence;
import proto.Number;


##
## Private Definitions
##

## Reports the given list of pending tokens as part of error processing.
fn reportError(pending) {
    note("Pending tokens:");

    $Generator::filterPump(pending, $Range::makeInclusiveRange(1, 50))
        { token, . -> note("    ", $Format::source(token)) };

    die("\nExtra tokens at end of program.")
};

## Set-like map of all lowercase identifier characters. Used to figure
## out if we're looking at a keyword in the `identifierString` rule.
def LOWER_ALPHA = {
    ($Range::makeInclusiveRange("a", "z"))*: true
};


##
## Layer 0 Rules
##
## This section consists of the definitions required to implement Layer 0,
## with comments indicating the "hooks" for higher layers. Subsections are
## used here to separate three related sets of rules (and hopefully make the
## grammar easier to follow).
##

## Forward declarations required for layer 2. These are all add-ons to
## layer 0 or 1 rules, used to expand the syntactic possibilities of the
## indicated base forms.
def parExpression2;
def parPostfixOperator2;
def parPrefixOperator2;
def parTerm2;

## Forward declaration required for integrating layer 1 definitions.
def parParser;

## Forward declarations.
def parAssignExpression;
def parRawClosure;

## Forward declaration for the "top" rule which parses operator expressions.
## This gets bound to `parUnaryExpression` in the Layer 0 and 1 grammars, but
## it's different in Layer 2.
def parOpExpression;

##
## Layer 0: Terms and expressions
##

## Parses an expression in general.
def parExpression = {:
    ## This one's the top level "regular-looking" expression (in that it
    ## covers the territory of C-style expressions).
    %parAssignExpression
|
    ## Note: Layer 2 adds additional rules here.
    %parExpression2
:};

## Parses a parenthesized expression. This produces a result identical to
## the inner `expression` node, except without an `interpolate` binding.
def parParenExpression = {:
    @"("
    ex = parExpression
    @")"

    { <> withoutInterpolate(ex) }
:};

## Parses a "name" of some sort. This is just an identifier, but with the
## result being the string payload (not wrapped in `@identifier(...)`).
def parName = {:
    nameIdent = @identifier
    { <> dataOf(nameIdent) }
:};

## Parses a non-empty comma-separated list of "names." A "name" is as per
## `parName` above. The result is a list of strings (per se).
def parNameList = {:
    first = parName
    rest = (@"," parName)*
    { <> [first, rest*] }
:};

## Parses a variable reference.
def parVarRef = {:
    name = parName
    { <> makeVarRefLvalue(name) }
:};

## Parses an integer literal. Note: This includes parsing a `-` prefix,
## so that simple negative constants aren't turned into complicated function
## calls.
def parInt = {:
    @"-"
    i = @int
    { <> makeLiteral($Number::neg(dataOf(i))) }
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
    name = parName
    { <> makeLiteral(name) }
|
    token = .
    {
        <> ifNot { dataOf(token) }
            {
                def type = get_typeName(token);
                def firstCh = nth(type, 0);
                <> ifIs { get(LOWER_ALPHA, firstCh) }
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
        <> ifIs { eq(keys, []) }
            { /out ->
                ## No keys were specified, so the value must be either a
                ## whole-map interpolation or a variable-name-to-its-value
                ## binding.
                ifValue { get_interpolate(value) }
                    { interp -> yield /out interp };
                ifIs { hasType(value, @@varRef) }
                    {
                        yield /out makeCall(REFS::makeValueMap,
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
            <> ifIs { eq(rest, []) }
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
        <> ifIs { eq(expressions, []) }
            { <> makeLiteral([]) }
            { <> makeCallOrApply(REFS::makeList, expressions*) }
    }
:};

## Parses a type literal form, yielding an expression node that produces a
## type value. If the name is a blatant literal, then the result of this rule
## is also a literal. If not, the result of this rule is a call to
## `makeDerivedDataType`.
def parType = {:
    @"@@"
    name = (parIdentifierString | parParenExpression)

    {
        <> ifIs { hasType(name, @@literal) }
            { <> makeLiteral(@@(get_nodeValue(name))) }
            { <> makeCall(REFS::makeDerivedDataType, name) }
    }
:};

## Parses a literal in derived value form.
def parDeriv = {:
    @"@"

    type = (
        name = parIdentifierString
        { <> makeLiteral(@@(get_nodeValue(name))) }
    |
        parParenExpression
    )

    value = (parParenExpression | parMap | parList)?

    { <> makeCall(REFS::makeValue, type, value*) }
:};

## Parses a closure, resulting in one that *always* has a `yield` binding.
def parFullClosure = {:
    ## The lookahead to makes it so we don't have to do any heavier-weight
    ## parsing in easy-failure cases.
    &@"{"
    raw = %parRawClosure

    {
        def closure = makeFullClosure(raw);
        <> withoutTops(closure)
    }
:};

## Parses a closure, resulting in one that does *not* necessarily have
## a `yield` binding.
def parBasicClosure = {:
    &@"{"  ## Lookahead for same reason as above.
    raw = %parRawClosure

    {
        def closure = makeBasicClosure(raw);
        <> withoutTops(closure)
    }
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
    c = parFullClosure

    {
        def formals = get_formals(c);
        ifIs { ne(formals, []) }
            { die("Invalid formal argument in code block.") };
        <> c
    }
:};

## Like, `parNullaryClosure` (above), except returning a basic (no
## required `yield`) closure node.
def parBasicNullaryClosure = {:
    c = parBasicClosure

    {
        def formals = get_formals(c);
        ifIs { ne(formals, []) }
            { die("Invalid formal argument in code block.") };
        <> c
    }
:};

## Parses a term (basic expression unit).
def parTerm = {:
    parVarRef | parInt | parString | parMap | parList |
    parDeriv | parType | parFullClosure | parParenExpression
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
    closureActuals = parFullClosure*
    { <> [closureActuals*, normalActuals*] }
|
    parFullClosure+
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
    { <> { node <> makeMaybeValue(node) } }
|
    ## Note: Layer 2 adds additional rules here.
    %parPostfixOperator2
:};

## Parses a unary expression. This is a term, optionally surrounded on
## either side by any number of unary operators. Postfix operators
## take precedence over (are applied before) the prefix operators.
def parUnaryExpression = {:
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
        { <> {base, prefixes: $Sequence::reverse(prefixes)} }
    )

    postfixes = parPostfixOperator*

    {
        def prefixes = basePrefixes::prefixes;
        var result = basePrefixes::base;

        $Generator::filterPump(postfixes) { op -> result := op(result) };
        $Generator::filterPump(prefixes) { op -> result := op(result) };
        <> result
    }
:};

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

##
## Layer 0: Statements and yields
##

## Parses a yield or a nonlocal exit (named or implicit). All of the forms
## matched by this rule necessarily appear at the end of a block and are
## expected to be placed in a `yield` binding of a `closure` node. The
## nonlocal exit forms all end up being represented as a `noYield` node, so
## it's sorta funny that they hang off of `yield` (because they never actually
## yield, per se).
def parYieldOrNonlocal = {:
    op = [@break @continue @return @yield]
    optQuest = @"?"?

    name = (
        { <> hasType(op, @@yield) }
        (
            @"/"
            parVarRef
        |
            ## Indicate that this is a regular (local) yield. Checked below.
            { <> @yield }
        )
    |
        { <> makeVarRef(get_typeName(op)) }
    )

    ## A value expression is mandatory if there is a `?` after the
    ## operator. Otherwise, it's optional.
    value = (
        v = parExpression
        { <> ifIs { optQuest* } { <> makeMaybe(v) } { <> v } }
    |
        { <> ifNot { optQuest* } { <> @void } }
    )

    {
        <> ifIs { eq(name, @yield) }
            { <> value }
            { <> makeNonlocalExit(name, value) }
    }
:};

## Parses a local yield / return. TODO: Remove!
def parOldYield = {:
    @"<>"
    (
        ex = parExpression
        { <> makeMaybe(ex) }
    |
        { <> @void }
    )
:};

## Parses an immutable variable definition, or forward declaration of same.
def parVarDef = {:
    @def
    name = parName
    optExpr = (@"=" parExpression)?

    { <> makeVarDef(name, optExpr*) }
:};

## Parses a mutable variable definition, or forward declaration of same.
def parVarDefMutable = {:
    @var
    name = parName
    optExpr = (@"=" parExpression)?

    { <> makeVarDefMutable(name, optExpr*) }
:};

## Parses a yield / nonlocal exit definition, yielding the def name.
def parYieldDef = {:
    @"/"
    name = parName
    { <> name }
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
        n = parName
        { <> {name: n} }
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
def parClosureDeclarations = {:
    most = (
        name = (
            n = parName
            { <> {name: n} }
        |
            { <> {} }
        )
        @"("
        formals = parFormalsList
        @")"
        { <> {name*, formals}}
    |
        formals = parFormalsList
        { <> {formals} }
    )

    yieldDef = parOptYieldDef

    (@"->" | &@"<>")

    { <> {most*, yieldDef*} }
|
    { <> {formals: []} }
:};

## Parses the common part of function definition and generic function binding.
## The result of this rule is a `@closure` node, translated along these lines:
##
## ```
## name(arg1, arg2) { /out -> stat1; stat2 }
## ```
## =>
## ```
## { name(arg1, arg2) /\"return" ->
##     def out = \"return";
##     stat1;
##     stat2
## }
## ```
##
## except without a yield def binding statement if an explicit yield def was
## not present.
def parFunctionCommon = {:
    name = parName
    @"("
    formals = parFormalsList
    @")"
    code = parBasicNullaryClosure

    {
        def returnDef = ifValue { code::yieldDef }
            { yieldDef ->
                ## The closure has a yield def, but we need to also bind
                ## it as `return`, so we add an extra local variable binding
                ## here.
                <> [makeVarDef(yieldDef, REFS::return)]
            }
            { <> [] };

        <> makeFullClosure({
            dataOf(code)*,
            formals,
            name,
            yieldDef: "return",
            statements: [returnDef*, get_statements(code)*]
        })
    }
:};

## Parses a `fn` definition statement. This wraps a `@closure` result of
## `parFunctionCommon` in a top-declaring `@varDef`.
def parFunctionDef = {:
    @fn
    closure = parFunctionCommon

    { <> withTop(makeVarDef(get_name(closure), closure)) }
:};


## Parses a generic function binding. This wraps a `@closure` result of
## `parFunctionCommon` in a `@call`. The closure also gets a new `this`
## formal argument.
def parGenericBind = {:
    @fn
    bind = (parVarRef | parType)
    @"."
    closure = parFunctionCommon

    {
        def formals = get_formals(closure);
        def name = get_name(closure);
        def fullClosure = withFormals(closure, [{name: "this"}, formals*]);
        <> makeCall(REFS::genericBind, makeVarRef(name), bind, fullClosure)
    }
:};

## Parses a generic function definition. The translation is along these lines:
##
## ```
## fn .name(arg1, arg2);
## ```
## =>
## ```
## def name;  ## At top of closure
## ...
## name := makeRegularGeneric("name", 2, 2);
## ```
##
## with different numbers depending on the shape of the arguments, and with
## the function `makeUnitypeGeneric` if a `*` precedes the `.name`.
def parGenericDef = {:
    @fn
    optStar = @"*"?
    @"."
    name = parName
    @"("
    formals = parFormalsList
    @")"

    {
        def fullFormals = [{}, formals*]; ## First one is `this`.
        def func = ifIs { eq(optStar, []) }
            { <> REFS::makeRegularGeneric }
            { <> REFS::makeUnitypeGeneric };
        def call = makeCall(
            func,
            makeLiteral(name),
            makeLiteral(formalsMinArgs(fullFormals)),
            makeLiteral(formalsMaxArgs(fullFormals)));

        <> withTop(makeVarDef(name, call))
    }
:};

## Parses an optional binding name or name prefix for an `import` statement.
## This rule never fails. The result is always a map, empty if there was no
## name / prefix, or binding one of `name` or `prefix`.
def parImportName = {:
    name = parName

    key = (
        @"*" { <> "prefix" }
    |
        { <> "name" }
    )

    @"="
    { <> {(key): name} }
|
    { <> {} }
:};

## Parses an optional format name for an `import` statement. This rule never
## fails. The result is always a map, empty if there was no format name, or
## binding `format`.
def parImportFormat = {:
    @"@"
    f = parIdentifierString
    { <> {format: get_nodeValue(f)} }
|
    { <> {} }
:};

## Parses the source for an `import` statement. The result is either a name
## string as the payload of either an `@internal` or `@external` value.
def parImportSource = {:
    @"."
    @"/"

    first = parName
    rest = (
        @"/"
        n = parName
        { <> cat("/", n) }
    )*
    optSuffix = (
        @"."
        n = parName
        { <> cat(".", n) }
    )?

    {
        def name = cat(first, rest*, optSuffix*);
        <> @internal(name)
    }
|
    first = parName
    rest = (
        @"."
        n = parName
        { <> cat(".", n) }
    )*

    {
        def name = cat(first, rest*);
        <> @external(name)
    }
:};

## Parses a list of binding names for an `import` statement. The result is
## a list of strings, or `@"*"` to indicate a wildcard of all names.
def parImportSelect = {:
    @"::"
    (
        @"*"
        { <> {select: @"*"} }
    |
        select = parNameList
        { <> {select} }
    )
|
    { <> {} }
:};

## Parses an `import` statement. This works by first parsing a very general
## version of the syntax which allows invalid combinations, and then checking
## in code that the actual combination is valid.
def parImportStatement = {:
    optExport = @export?
    @import
    nameOrPrefix = parImportName
    format = parImportFormat
    source = parImportSource
    select = parImportSelect

    {
        def data = {nameOrPrefix*, format*, select*, source};
        <> ifIs { optExport* }
            { <> makeExport(makeImport(data)) }
            { <> makeImport(data) }
    }
:};

## Parses an executable statement form that is `export`able. This does *not*
## include `import` statements.
def parExportableStatement = {:
    parFunctionDef | parGenericDef | parVarDef
:};

## Parses an executable statement form (direct closure / program element).
## This includes all the `export`able statements and a few additional forms.
def parStatement = {:
    parExportableStatement | parGenericBind | parVarDefMutable | parExpression
:};

## Parses a program body statement form, including both regular executable
## statements and `export` statements (but not `import` statements).
def parProgramStatement = {:
    parStatement
|
    @export
    (
        select = parNameList
        { <> makeExportSelection(select*) }
    |
        stat = parExportableStatement
        { <> makeExport(stat) }
    )
:};

##
## Layer 0: Closures and programs
##

## Parses a closure body (statements plus optional yield).
def parClosureBody = {:
    @";"*

    most = (
        s = parStatement
        @";"+
        { <> s }
    )*

    last = (
        s = parStatement
        { <> {statements: [s]} }
    |
        y = (parYieldOrNonlocal | parOldYield)
        { <> {statements: [], yield: y} }
    |
        { <> {statements: []} }
    )

    @";"*

    { <> {last*, statements: [most*, last::statements*]} }
:};

## Parses a closure (in-line anonymous function, with no extra bindings).
## This results in a simple map of bindings.
parRawClosure := {:
    @"{"
    decls = parClosureDeclarations
    body = parClosureBody
    @"}"
    { <> {decls*, body*} }
:};

## Parses a program (list of statements, including imports and exports).
def parProgram = {:
    imports = (
        @";"*
        first = parImportStatement
        rest = (@";"+ parImportStatement)*
        { <> [first, rest*] }
    |
        { <> [] }
    )

    body = (
        (
            ## There was at least one import, so there needs to be at least
            ## one semicolon between the final import and first statement.
            { <> ne(imports, []) }
            @";"+
        |
            @";"*
        )

        first = parProgramStatement
        rest = (@";"+ parProgramStatement)*
        { <> [first, rest*] }
    |
        { <> [] }
    )

    @";"*

    {
        def closure = makeFullClosure({
            statements: [imports*, body*],
            yield:      @void
        });
        <> withoutTops(closure)
    }
:};

##
## Layer 0: Entry points
##

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
    @@"&": @@lookaheadSuccess,
    @@"!": @@lookaheadFailure,
    @@"?": @@opt,
    @@"*": @@star,
    @@"+": @@plus
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
    { <> @token(@@(get_nodeValue(type))) }
:};

## Parses a string or character range parsing expression, used when defining
## sets. Yields a string per se (not a token).
def parParserSetString = {:
    s = @string

    (
        @".."
        end = @string

        { /out ->
            def startChar = dataOf(s);
            def endChar = dataOf(end);

            ## Reject non-single-character strings.
            ifIs { ne(1, get_size(startChar)) } { yield /out };
            ifIs { ne(1, get_size(endChar)) } { yield /out };

            <> cat($Range::makeInclusiveRange(startChar, endChar)*)
        }
    |
        { <> dataOf(s) }
    )
:};

## Parses a set (or set complement) parsing expression.
def parParserSet = {:
    @"["

    type = (
        @"!" { <> @@tokenSetComplement }
    |
        { <> @@tokenSet }
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
        name = parName
        @"="
        pex = parLookaheadPex
        { <> @varDef{name, value: pex} }
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

## For more details, see comment where `parOpExpression` is declared, above.
parOpExpression := parUnaryExpression;

## In layer 2, these are all non-trivial, but here they are simply
## always-fail.

parExpression2      := {: !() :};
parPostfixOperator2 := {: !() :};
parPrefixOperator2  := {: !() :};
parTerm2            := {: !() :};
```
