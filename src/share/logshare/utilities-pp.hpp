//
//  utilities-pp.hpp
//  tool8-separate
//
//  Created by Philip Koch on 4/26/16.
//

#ifndef utilities_pp_h
#define utilities_pp_h

#if defined(NBL_UTILITIES_VERSION) && NBL_UTILITIES_VERSION != 2
#error "wrong NBL_UTILITIES_VERSION"
#endif

#define NBL_UTILITIES_VERSION 2

/****************************************************/
/* Basic always-defined macros for general utility. */

#define _NBUQ(thing) ::nbl::utilities::thing

#define NBL_INFO_LEVEL _NBUQ(InfoLevel)
#define NBL_WARN_LEVEL _NBUQ(WarnLevel)
#define NBL_ERROR_LEVEL _NBUQ(ErrorLevel)
#define NBL_ALWAYS_LEVEL _NBUQ(AlwaysLevel)

#define NBL_GLUE_2(a,b) NBL_GLUE_2_(a,b)
#define NBL_GLUE_2_(a,b) a##b

#define NBL_PRINT(format, ...) NBL_PRINT_(NBL_ALWAYS_LEVEL, format, ## __VA_ARGS__)

#define NBL_PRINT_(LEVEL, format, ...)                  \
    do { _NBUQ(nbl_print)(                                     \
        NBL_LOGGING_LEVEL,                                   \
        LEVEL,                                          \
        __FILE__,                                       \
        __LINE__,                                       \
        format, ## __VA_ARGS__ );                       \
    } while(0);

//Return (const char *) s.t. NBL_CSTR_DESCRIBE(false) => "var 'false' == 0"
#define NBL_CSTR_DESCRIBE(VAR) ( _NBUQ(describe)(#VAR, VAR).c_str() )

#define NBL_STRINGIFY(SYM) #SYM
#define NBL_STRINGIFY_EXPAND(SYM) NBL_STRINGIFY(SYM)

//If statement, always evaluates expr and if NZ return prints perror string and allows
//a following if statement body.
#define PERROR_IF(expr) if ( expr && _NBUQ(safe_perror_describe)(errno, __FILE__, __LINE__, #expr ) )

//Prevent compiler from auto-generating copy constructor/assign for type cname
#define NBL_DISALLOW_COPY(cname)                 \
    public:                                      \
    cname(cname const&)          = delete;   \
    void operator=(cname const&) = delete;   \

/* for use inside a class */
#define NBL_OSTREAM_FRIEND( type, iname, instance_rep )  \
    friend std::ostream& operator<<(std::ostream&os, const type& iname) {    \
        os << instance_rep ; return os; }

/* for use on a non-class type, such as enum */
#define NBL_OSTREAM_OVERLOAD( type, iname, instance_rep ) \
    inline std::ostream& operator<<(std::ostream&os, const type& iname) {    \
        os << instance_rep ; return os; }

/**********************************************************/
/* Debugging macros, define NBL_NO_DEBUG to evaluate      */
/* LOG, WHATIS, CHECK, and ASSERT statments to whitespace */

#ifndef NBL_NO_DEBUG

//Formatted and colored output conditioned on (LEVEL >= current_level)
#define NBL_LOG(LEVEL, format, ...) NBL_PRINT_(LEVEL, format, ## __VA_ARGS__)

#define NBL_CONDITIONED(predicate, action) \
    for(auto _nbl_cond_ = (predicate) ; _nbl_cond_

#define NBL_LOG_IF_(LEVEL, CONDITION, format, ...)  \
    if( CONDITION ) { NBL_LOG(LEVEL, format, ## __VA_ARGS__)
//Formatted and colored ouput conditioned on (LEVEL >= current_level) && CONDITION
#define NBL_LOG_IF(LEVEL, CONDITION, format, ...)   \
    NBL_LOG_IF_(LEVEL, CONDITION, format, ## __VA_ARGS__) }

//Print out state of VAR (always)
#define NBL_WHATIS(VAR) NBL_LOG(NBL_ALWAYS_LEVEL, "%s", NBL_CSTR_DESCRIBE(VAR) )

#define NBL_WHAT(msg, VAR) NBL_LOG(NBL_ALWAYS_LEVEL, "%s: %s", msg, NBL_CSTR_DESCRIBE(VAR) )

//Warn if EXPRESSION evaluates to false
#define NBL_CHECK(EXPRESSION) NBL_LOG_IF(NBL_WARN_LEVEL, !(EXPRESSION), "CHECK of '%s' FAILED (0)!", #EXPRESSION )

//Warn if (A OP B) evaluates to false, print values
#define NBL_CHECK_TWO(A, B, OP) NBL_LOG_IF(NBL_WARN_LEVEL, !((A) OP (B)), "CHECK of '%s %s %s' FAILED!\n\t%s\n\t%s",  \
    #A, #OP, #B, NBL_CSTR_DESCRIBE(A), NBL_CSTR_DESCRIBE(B) )

//Assert EXPRESSION
#define NBL_ASSERT(EXPRESSION) NBL_LOG_IF_(NBL_ERROR_LEVEL, !(EXPRESSION), "ASSERT of '%s' FAILED (0)!\n", #EXPRESSION ) assert(false); }

//Assert (A OP B), print values
#define NBL_ASSERT_TWO(A, B, OP) NBL_LOG_IF_(NBL_ERROR_LEVEL, !((A) OP (B)), "ASSERT of '%s %s %s' FAILED!\n\t%s\n\t%s\n",  \
    #A, #OP, #B, NBL_CSTR_DESCRIBE(A), NBL_CSTR_DESCRIBE(B) ) assert(false); }

#else   //NBL_NO_DEBUG
        //Define as blank space.
#define NBL_LOG(LEVEL, format, ...)
#define NBL_LOG_IF(LEVEL, CONDITION, format, ...)
#define NBL_WHATIS(VAR)
#define NBL_WHAT(msg, VAR)
#define NBL_CHECK(EXPRESSION)
#define NBL_CHECK_TWO(A, B, OP)
#define NBL_ASSERT(EXPRESSION)
#define NBL_ASSERT_TWO(A, B, OP)

#endif  //NBL_NO_DEBUG

//Common values for OP
#define NBL_CHECK_EQ(A, B) NBL_CHECK_TWO(A, B, ==)
#define NBL_CHECK_NE(A, B) NBL_CHECK_TWO(A, B, !=)
#define NBL_CHECK_LT(A, B) NBL_CHECK_TWO(A, B, <)
#define NBL_CHECK_LE(A, B) NBL_CHECK_TWO(A, B, <=)
#define NBL_CHECK_GT(A, B) NBL_CHECK_TWO(A, B, >)
#define NBL_CHECK_GE(A, B) NBL_CHECK_TWO(A, B, >=)

//Common values for OP
#define NBL_ASSERT_EQ(A, B) NBL_ASSERT_TWO(A, B, ==)
#define NBL_ASSERT_NE(A, B) NBL_ASSERT_TWO(A, B, !=)
#define NBL_ASSERT_LT(A, B) NBL_ASSERT_TWO(A, B, <)
#define NBL_ASSERT_LE(A, B) NBL_ASSERT_TWO(A, B, <=)
#define NBL_ASSERT_GT(A, B) NBL_ASSERT_TWO(A, B, >)
#define NBL_ASSERT_GE(A, B) NBL_ASSERT_TWO(A, B, >=)

//Shortcut macros
#define NBL_INFO(format, ...) NBL_LOG(NBL_INFO_LEVEL, format, ## __VA_ARGS__)
#define NBL_WARN(format, ...) NBL_LOG(NBL_WARN_LEVEL, format, ## __VA_ARGS__)
#define NBL_ERROR(format, ...) NBL_LOG(NBL_ERROR_LEVEL, format, ## __VA_ARGS__)

/****************************************************/
/* PP macros */
/* these macros use some PP tricks to implement additional macro functionality.
 * typically program readability is improved while the macro definition itself is depressingly esoteric
 */

#define NBL_COMPILER_ASSERT(msg, expr)   \
    NBL_COMPILER_ASSERT_(msg, expr, __LINE__)

//Num args must be > 0, evaluates to num args as int
//  (works by pushing a series of 64 ints forward by NARGS, then evaluating to the 64th)
#define NBL_NARG(...) NBL_NARG_( __VA_ARGS__, NBL_R_SEQ_64() )

//does macro_action on each argument, must have >0 # args
#define NBL_FOR_EACH(macro_action, ...)  \
    NBL_FOR_EACH_CHOOSE(__VA_ARGS__, NBL_FOR_EACH_SET)(macro_action, __VA_ARGS__)

#define NBL_MAKE_ENUM(EnumName, ...) enum EnumName { NBL_FOR_EACH(NBL_ENUM_ONE, __VA_ARGS__, EnumName ## _COUNT ) };

#define NBL_MAKE_STRINGS(VarName, ...) const char * VarName[] = { NBL_FOR_EACH(NBL_STRING_ONE, __VA_ARGS__) };
#define NBL_DECLARE_STRINGS(VarName) extern const char * VarName[];
#define NBL_MAKE_STATIC_STRINGS(VarName, ...) static const char * VarName[] = { NBL_FOR_EACH(NBL_STRING_ONE, __VA_ARGS__) };

#define NBL_MAKE_FLAGS(EnumName, ...)   \
    NBL_COMPILER_ASSERT(enum_made_with_NBL_MAKE_ENUM, (NBL_NARG(__VA_ARGS__) == EnumName ## _COUNT)) \
    typedef uint64_t EnumName ## Mask;    \
    enum EnumName ## FlagType { NBL_FOR_EACH(NBL_FLAG_ONE, __VA_ARGS__)  \
    EnumName ## AllFlags = ((1 << EnumName ## _COUNT) - 1) };

#define NBL_MAKE_ENUM_FULL(EnumName, ...)                       \
    NBL_MAKE_ENUM(EnumName, __VA_ARGS__)                        \
    NBL_MAKE_STATIC_STRINGS(EnumName ## Strings, __VA_ARGS__)   \
    NBL_MAKE_FLAGS(EnumName, __VA_ARGS__)

/****************************************************/
/* Basic always defined macros for general utility. */
/******************************************/
/* HELPER MACROS, NOT TO BE USED DIRECTLY */

#define NBL_GLUE_3(a1, a2, a3) NBL_GLUE_3_(a1, a2, a3)
#define NBL_GLUE_3_(a1, a2, a3) a1 ## a2 ## a3

#define NBL_COMPILER_ASSERT_(msg, expr, line)  \
    typedef char NBL_GLUE_3(msg, _compiler_assertion_failed_, line) [(expr) ? (+1) : (-1)];

#define NBL_NARG_(...) NBL_GET_64TH(__VA_ARGS__)

//Evaluates to 64th argument, ignores the rest
#define NBL_GET_64TH(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63, N,...) N

#define NBL_R_SEQ_64() 63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

#define NBL_FOR_EACH_CHOOSE(...) NBL_GET_64TH(__VA_ARGS__)

#define NBL_STRING_ONE(value) #value,
#define NBL_ENUM_ONE(value) value,
#define NBL_FLAG_ONE(bitLeft) bitLeft ## Flag = (1 << bitLeft) ,

//FOREACH MACRO SET
#define NBL_FOR_EACH_SET NBL_FE_63,NBL_FE_62,NBL_FE_61,NBL_FE_60,NBL_FE_59,NBL_FE_58,NBL_FE_57,NBL_FE_56,NBL_FE_55,NBL_FE_54,NBL_FE_53,NBL_FE_52,NBL_FE_51,NBL_FE_50,NBL_FE_49,NBL_FE_48,NBL_FE_47,NBL_FE_46,NBL_FE_45,NBL_FE_44,NBL_FE_43,NBL_FE_42,NBL_FE_41,NBL_FE_40,NBL_FE_39,NBL_FE_38,NBL_FE_37,NBL_FE_36,NBL_FE_35,NBL_FE_34,NBL_FE_33,NBL_FE_32,NBL_FE_31,NBL_FE_30,NBL_FE_29,NBL_FE_28,NBL_FE_27,NBL_FE_26,NBL_FE_25,NBL_FE_24,NBL_FE_23,NBL_FE_22,NBL_FE_21,NBL_FE_20,NBL_FE_19,NBL_FE_18,NBL_FE_17,NBL_FE_16,NBL_FE_15,NBL_FE_14,NBL_FE_13,NBL_FE_12,NBL_FE_11,NBL_FE_10,NBL_FE_9,NBL_FE_8,NBL_FE_7,NBL_FE_6,NBL_FE_5,NBL_FE_4,NBL_FE_3,NBL_FE_2,NBL_FE_1

#define NBL_FE_1(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)
#define NBL_FE_2(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_1(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_3(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_2(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_4(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_3(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_5(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_4(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_6(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_5(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_7(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_6(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_8(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_7(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_9(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_8(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_10(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_9(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_11(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_10(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_12(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_11(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_13(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_12(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_14(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_13(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_15(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_14(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_16(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_15(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_17(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_16(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_18(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_17(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_19(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_18(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_20(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_19(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_21(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_20(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_22(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_21(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_23(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_22(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_24(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_23(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_25(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_24(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_26(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_25(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_27(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_26(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_28(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_27(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_29(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_28(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_30(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_29(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_31(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_30(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_32(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_31(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_33(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_32(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_34(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_33(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_35(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_34(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_36(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_35(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_37(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_36(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_38(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_37(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_39(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_38(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_40(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_39(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_41(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_40(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_42(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_41(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_43(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_42(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_44(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_43(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_45(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_44(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_46(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_45(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_47(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_46(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_48(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_47(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_49(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_48(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_50(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_49(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_51(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_50(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_52(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_51(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_53(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_52(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_54(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_53(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_55(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_54(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_56(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_55(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_57(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_56(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_58(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_57(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_59(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_58(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_60(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_59(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_61(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_60(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_62(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_61(MACRO_ACTION, __VA_ARGS__)
#define NBL_FE_63(MACRO_ACTION, MACRO_ARGUMENT, ...) MACRO_ACTION(MACRO_ARGUMENT)NBL_FE_62(MACRO_ACTION, __VA_ARGS__)

#include "utilities.hpp"

#endif /* utilities_pp_h */
