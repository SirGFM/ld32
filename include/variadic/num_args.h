#ifndef NUM_ARGS_H
#define NUM_ARGS_H


/**
 * PP_NARG returns the number of arguments passed to it,
 * to a maximum of 10 arguments.
 *
 * Note that the argument list MUST end in a comma
 * to properly detect when no arguments are passed.
 *
 * E.g.:
 *
 * 	PP_NARG(arg_1, arg_2,) // Resolves to 2
 * 	PP_NARG()              // Resolves to 0
 *
 * Source:
 *   https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
 */
#define PP_NARG(...) PP_NARG_(__VA_ARGS__ PP_RSEQ_N())


/**
 * PP_NARG_ merges the variadic list of arguments
 * supplied to PP_NARG with a premade list counting the number of arguments.
 *
 * Assuming that at most N arguments are accepted,
 * this causes the Nth argument to be the number of arguments passed.
 */
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)


/**
 * PP_ARG_N retrieves the 10th argument supplied to this macro,
 * Which should be the number of arguments passed to PP_NARG.
 */
#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N


/** List with the maximum number of variadic arguments acceptable. */
#define PP_RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0


#endif /* NUM_ARGS_H */
