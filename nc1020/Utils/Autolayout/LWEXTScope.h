//
//  EXTScope.h
//  extobjc
//
//  Created by Justin Spahr-Summers on 2011-05-04.
//  Copyright (C) 2012 Justin Spahr-Summers.
//  Released under the MIT license.
//

#import "LWMetamacros.h"

/**
 * \@onExit defines some code to be executed when the current scope exits. The
 * code must be enclosed in braces and terminated with a semicolon, and will be
 * executed regardless of how the scope is exited, including from exceptions,
 * \c goto, \c return, \c break, and \c continue.
 *
 * Provided code will go into a block to be executed later. Keep this in mind as
 * it pertains to memory management, restrictions on assignment, etc. Because
 * the code is used within a block, \c return is a legal (though perhaps
 * confusing) way to exit the cleanup block early.
 *
 * Multiple \@onExit statements in the same scope are executed in reverse
 * lexical order. This helps when pairing resource acquisition with \@onExit
 * statements, as it guarantees teardown in the opposite order of acquisition.
 *
 * @note This statement cannot be used within scopes defined without braces
 * (like a one line \c if). In practice, this is not an issue, since \@onExit is
 * a useless construct in such a case anyways.
 */
#define lw_onExit \
    __strong lw_cleanupBlock_t lw_metamacro_concat(lw_exitBlock_, __LINE__) __attribute__((cleanup(lw_executeCleanupBlock), unused)) = ^

/**
 * Creates \c __weak shadow variables for each of the variables provided as
 * arguments, which can later be made strong again with #strongify.
 *
 * This is typically used to weakly reference variables in a block, but then
 * ensure that the variables stay alive during the actual execution of the block
 * (if they were live upon entry).
 *
 * See #strongify for an example of usage.
 */
#define lw_weakify(...) \
    lw_metamacro_foreach_cxt(lw_weakify_,, __weak, __VA_ARGS__)

/**
 * Like #weakify, but uses \c __unsafe_unretained instead, for targets or
 * classes that do not support weak references.
 */
#define lw_unsafeify(...) \
    lw_metamacro_foreach_cxt(lw_weakify_,, __unsafe_unretained, __VA_ARGS__)

/**
 * Strongly references each of the variables provided as arguments, which must
 * have previously been passed to #weakify.
 *
 * The strong references created will shadow the original variable names, such
 * that the original names can be used without issue (and a significantly
 * reduced risk of retain cycles) in the current scope.
 *
 * @code

    id foo = [[NSObject alloc] init];
    id bar = [[NSObject alloc] init];

    @weakify(foo, bar);

    // this block will not keep 'foo' or 'bar' alive
    BOOL (^matchesFooOrBar)(id) = ^ BOOL (id obj){
        // but now, upon entry, 'foo' and 'bar' will stay alive until the block has
        // finished executing
        @strongify(foo, bar);

        return [foo isEqual:obj] || [bar isEqual:obj];
    };

 * @endcode
 */
#define lw_strongify(...) \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wshadow\"") \
    lw_metamacro_foreach(lw_strongify_,, __VA_ARGS__) \
    _Pragma("clang diagnostic pop")

/*** implementation details follow ***/
typedef void (^lw_cleanupBlock_t)(void);

#if defined(__cplusplus)
extern "C" {
#endif
    void lw_executeCleanupBlock (__strong lw_cleanupBlock_t *block);
#if defined(__cplusplus)
}
#endif

#define lw_weakify_(INDEX, CONTEXT, VAR) \
    CONTEXT __typeof__(VAR) lw_metamacro_concat(VAR, _weak_) = (VAR);

#define lw_strongify_(INDEX, VAR) \
    __strong __typeof__(VAR) VAR = lw_metamacro_concat(VAR, _weak_);
