/*
 * Compiler for low-layer data structures. Emits C code.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Error Messages
 */

/** The name of this executable. Should be set in `main()`. */
static char *programName = "<unknown>";

/**
 * Dies with the given message, printf-style.
 */
static void die(const char *format, ...)
    __attribute__((noreturn))
    __attribute__((format (printf, 1, 2)));
static void die(const char *format, ...) {
    if (strcmp(format, "errno") == 0) {
        perror(programName);
    } else {
        fprintf(stderr, "%s: ", programName);

        va_list rest;
        va_start(rest, format);
        vfprintf(stderr, format, rest);
        va_end(rest);

        fputs("\n", stderr);
    }

    exit(1);
}


/*
 * Low Level Parse State
 */

/**
 * Parse state.
 */
typedef struct {
    /** Current buffer position. */
    char *at;

    /** Current line number. */
    size_t line;

    /** Name of the file. */
    const char *fileName;
} ParseState;

/**
 * Initializes a parse state.
 */
static void parseInit(ParseState *state, const char *fileName, char *buf) {
    if (strcmp(fileName, "-") == 0) {
        fileName = "<stdin>";
    }

    state->fileName = fileName;
    state->at = buf;
    state->line = 1;
}

/**
 * Dies with a parse error.
 */
static void parseDie(ParseState *state, const char *format, ...)
    __attribute__((noreturn));
static void parseDie(ParseState *state, const char *format, ...) {
    char *fileLine = NULL;
    char *msg = NULL;

    asprintf(&fileLine, "%s, line %zu", state->fileName, state->line);

    va_list rest;
    va_start(rest, format);
    vasprintf(&msg, format,rest);
    va_end(rest);

    if ((fileLine == NULL) || (msg == NULL)) {
        die("Trouble making error message. Eek!");
    }

    die("%s: %s", fileLine, msg);
}

/**
 * Peeks ahead N bytes, zero-based. Note: Doesn't notice if you try to
 * peek past EOF.
 */
static char parsePeekNth(ParseState *state, size_t n) {
    char c = state->at[n];

    if ((unsigned char) c >= (unsigned char) 0x7f) {
        parseDie(state, "Non-ASCII character.");
    } else if ((c < ' ') && (c != '\n') && (c != '\0')) {
        parseDie(state, "Non-newline control character.");
    }

    return c;
}

/**
 * Peeks at the next byte.
 */
static char parsePeek(ParseState *state) {
    return parsePeekNth(state, 0);
}

/**
 * Reads (consumes) the next byte.
 */
static char parseRead(ParseState *state) {
    char c = parsePeek(state);

    if (c != '\0') {
        state->at++;
        if (c == '\n') {
            state->line++;
        }
    }

    return c;
}

/**
 * Indicates if the state is currently at EOF.
 */
static bool parseEof(ParseState *state) {
    return (parsePeek(state) == '\0');
}

/**
 * Skips the next N bytes.
 */
static void parseSkip(ParseState *state, size_t n) {
    while (n != 0) {
        parseRead(state);
        n--;
    }
}


/*
 * Parsing Per Se
 */

/**
 * Converts a hex character to its value.
 */
static int hexValue(char c) {
    if ((c >= '0') && (c <= '9')) {
        return (int) c - '0';
    } else if ((c >= 'a') && (c <= 'z')) {
        return (int) c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'Z')) {
        return (int) c - 'A' + 10;
    } else {
        die("Bad digit: \"%c\"\n", c);
    }
}

/**
 * Skips whitespace, including comments.
 */
static void skipWhitespace(ParseState *state) {
    for (;;) {
        char c = parsePeek(state);
        if (c == '#') {
            while (c != '\n') {
                c = parseRead(state);
            }
        } else if ((c != ' ') && (c != '\n')) {
            break;
        } else {
            parseRead(state);
        }
    }
}

/**
 * Skips exactly the given character.
 */
static void skipChar(ParseState *state, char c) {
    if (parsePeek(state) != c) {
        parseDie(state, "Unexpected character: \"%c\"", c);
    }

    parseRead(state);
}

/**
 * "Compiles" a name (identifier).
 */
static void compileName(ParseState *state, bool isDefine) {
    bool first = true;

    if (!isDefine) {
        printf("samRef(");
    }

    printf("samName(\"");

    for (;;) {
        char c = parsePeek(state);
        if (!(((c >= 'a') && (c <= 'z')) ||
              ((c >= 'A') && (c <= 'Z')) ||
              (c == '_') ||
              ((c >= '0') && (c <= '9') && !first))) {
            break;
        }
        fputc(c, stdout);
        parseRead(state);
        first = false;
    }

    printf("\")");

    if (!isDefine) {
        printf(")");
    }

    if (first) {
        parseDie(state, "Expected name.");
    }
}

/* Defined below. */
static void compileValue(ParseState *state);

/**
 * Compiles a stringlet (listlet of intlets). Assumes the opening
 * `@` is already read.
 */
static void compileStringlet(ParseState *state) {
    skipChar(state, '\"');

    printf("samListlet(");

    bool first = true;
    for (;;) {
        char c = parseRead(state);
        if (c == '\"') {
            break;
        }

        if (first) {
            first = false;
        } else {
            printf(", ");
        }

        if (c == '\\') {
            c = parseRead(state);
            switch (c) {
                case '\"': { break; }
                case '\\': { break; }
                case 'n':  { c = '\n'; break; }
                default: { parseDie(state, "Illegal escape."); }
            }
        }

        printf("samIntletFromByte(%d)", (int) c);
    }

    if (!first) {
        printf(", ");
    }

    printf("NULL)");
}

/**
 * Compiles an intlet. Assumes the opening `@` is already read.
 */
static void compileIntlet(ParseState *state) {
    char c = parsePeek(state);
    bool neg = false;

    if (c == '-') {
        neg = true;
        parseRead(state);
    }

    size_t count = 0;
    for (;;) {
        c = parsePeekNth(state, count);
        if (!isxdigit(c)) {
            break;
        }
        count++;
    }

    if (count == 0) {
        parseDie(state, "Expected int value.");
    }

    printf("samIntlet(%s", neg ? "true" : "false");

    for (int i = count - 1; i >= 0; i -= 2) {
        int low = hexValue(parsePeekNth(state, i));
        int high = (i == 0) ? 0 : hexValue(parsePeekNth(state, i - 1));
        printf(", 0x%02x", low | (high << 4));
    }

    printf(", -1)");
    parseSkip(state, count);
}

/**
 * Compiles a listlet. Assumes the opening `@` is already read.
 */
static void compileListlet(ParseState *state) {
    skipChar(state, '[');

    printf("samListlet(");

    bool first = true;
    for (;;) {
        skipWhitespace(state);
        if (parsePeek(state) == ']') {
            break;
        }

        if (first) {
            first = false;
        } else {
            printf(", ");
            skipChar(state, ',');
        }

        compileValue(state);
    }

    if (!first) {
        printf(", ");
    }

    printf("NULL)");
    skipChar(state, ']');
}

/**
 * Compiles a maplet. Assumes the opening `@` is already read.
 */
static void compileMaplet(ParseState *state) {
    skipChar(state, '{');

    printf("samMaplet(");

    bool first = true;
    for (;;) {
        skipWhitespace(state);
        if (parsePeek(state) == '}') {
            break;
        }

        if (first) {
            first = false;
        } else {
            printf(", ");
            skipChar(state, ',');
        }

        compileValue(state);
        printf(", ");
        skipWhitespace(state);
        skipChar(state, ':');
        compileValue(state);
    }

    if (!first) {
        printf(", ");
    }

    printf("NULL)");
    skipChar(state, '}');
}

/**
 * "Compiles" a uniquelet. Assumes the opening `@` is already read.
 */
static void compileUniquelet(ParseState *state) {
    skipChar(state, '@');
    printf("samUniquelet()");
}

/**
 * Compiles a value.
 */
static void compileValue(ParseState *state) {
    skipWhitespace(state);

    if (parsePeek(state) == '@') {
        // It's a literal.
        skipChar(state, '@');
        char c = parsePeek(state);
        switch (c) {
            case '\"': { compileStringlet(state); break; }
            case '[':  { compileListlet(state);   break; }
            case '{':  { compileMaplet(state);    break; }
            case '@':  { compileUniquelet(state); break; }
            default:   { compileIntlet(state);    break; }
        }
    } else {
        // It had better be a name reference.
        compileName(state, false);
    }
}

/**
 * Skips an "assign" token.
 */
static void skipAssign(ParseState *state) {
    skipWhitespace(state);
    skipChar(state, ':');
    skipChar(state, '=');
}

/**
 * Compiles / translates the given file contents.
 */
static void compile(ParseState *state) {
    for (;;) {
        skipWhitespace(state);
        if (parseEof(state)) {
            break;
        }

        printf("samDefine(");
        compileName(state, true);
        printf(",\n    ");
        skipAssign(state);
        compileValue(state);
        skipWhitespace(state);
        skipChar(state, ';');
        printf(");\n\n");
    }
}


/*
 * High Level Control
 */

/**
 * Reads the entirety of a file into the given buffer.
 */
static void readFile(const char *fileName, char *buf, size_t bufSize) {
    if (strcmp(fileName, "-") == 0) {
        fileName = "/dev/stdin";
    }

    FILE *f = fopen(fileName, "r");

    if (f == NULL) {
        die("errno");
    }

    size_t amt = fread(buf, 1, bufSize - 1, f);

    if (ferror(f)) {
        die("errno");
    }

    if (!feof(f)) {
        die("File too large: %s", fileName);
    }

    buf[amt] = '\0';
}

/**
 * Processes a single file. Reads it in, and processes it by recursive
 * descent.
 */
static void processFile(const char *fileName) {
    char buf[10000];
    ParseState state;

    readFile(fileName, buf, sizeof(buf));
    parseInit(&state, fileName, buf);
    compile(&state);
}

/**
 * Runs the show.
 */
int main(int argc, char **argv) {
    programName = *argv;

    argv++;

    while (*argv) {
        if (strcmp(*argv, "--") == 0) {
            argv++;
            break;
        } else if (strcmp(*argv, "--help") == 0) {
            printf("%s [--] [<file-name> ...]\n", programName);
            exit(0);
        } else if (**argv == '-') {
            die("Unknown option: %s", *argv);
        } else {
            break;
        }
    }

    if (*argv == NULL) {
        processFile("-");
    } else {
        while (*argv != NULL) {
            processFile(*argv);
            argv++;
        }
    }
}
