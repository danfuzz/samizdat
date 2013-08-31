/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Simple C-based module system
 */

#ifndef _MODULE_H_
#define _MODULE_H_


/*
 * Private definitions
 */

/** Prototype for a module initialization function. */
typedef void (*zmodInitFunction)(void);

/** Possible statuses of module initialization. */
typedef enum {
    MOD_UNINITIALIZED,
    MOD_INITIALIZING,
    MOD_INITIALIZED
} zmodStatus;

/**
 * Implementation function behind `MOD_USE`.
 */
void modUse(const char *name, zmodStatus *status, zmodInitFunction func);


/*
 * Public definitions
 */

/**
 * Defines a module inititialization function. Use this instead of
 * a regular function declaration.
 */
#define MOD_INIT(name) \
    zmodStatus MOD_STATUS_##name = MOD_UNINITIALIZED; \
    void MOD_INIT_##name(void)

/**
 * Indicates that the named module is required by subsequent code.
 * Generally placed inside the initialization function of some other
 * module.
 */
#define MOD_USE(name) \
    do { \
        extern zmodStatus MOD_STATUS_##name; \
        void MOD_INIT_##name(void); \
        modUse(#name, &MOD_STATUS_##name, MOD_INIT_##name); \
    } while(0)

#endif
