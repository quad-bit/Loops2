#pragma once

#include <iostream>

#define ASSERT_MSG(Expr, Msg) \
        AssertWithMsg(#Expr, Expr, __FILE__, __LINE__, Msg)

#ifndef NDEBUG
    #define ASSERT_MSG_DEBUG(Expr, Msg) \
        AssertWithMsg(#Expr, Expr, __FILE__, __LINE__, Msg)

    #define ASSERT(Expr) \
        Assert(#Expr, Expr, __FILE__, __LINE__)

    #define DEPRECATED ASSERT_MSG_DEBUG(0, "Deprecated") 
    
#else
    #define ASSERT_MSG(Expr, Msg) ;
    #define ASSERT(Expr);
    #define DEPRECATED ; 
#endif

void AssertWithMsg(const char* expr_str, bool expr, const char* file, int line, const char* msg);

void Assert(const char* expr_str, bool expr, const char* file, int line);

