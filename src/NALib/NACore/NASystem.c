
// This file is part of NALib, a collection of C and C++ source code
// intended for didactical purposes. Full license notice at the bottom.

#include "../NASystem.h"




#ifndef NDEBUG

  // The error printing method. Errors will be emitted to the stderr output.
  // When NDEBUG is defined, these functions are OBSOLETE!
  #ifdef __cplusplus
    #include <cstdio>
  #else
    #include <stdio.h>
  #endif


  void naError(const char* functionsymbol, const char* text, ...){
    va_list argumentlist;
    va_start(argumentlist, text);
    // Set a breakpoint here, if everything fails.
    fprintf(stderr, "Error in %s: ", functionsymbol);
    vfprintf(stderr, text, argumentlist);
    fprintf(stderr, "\n");
    va_end(argumentlist);
  }



  NA_NORETURN void naCrash(const char* functionsymbol, const char* text, ...){
    va_list argumentlist;
    va_start(argumentlist, text);
    // Set a breakpoint here, if everything fails.
    fprintf(stderr, "Critical Error in %s: ", functionsymbol);
    vfprintf(stderr, text, argumentlist);
    fprintf(stderr, "\nCrashing the application deliberately...\n");
    va_end(argumentlist);
    exit(EXIT_FAILURE);
  }


#endif




// Copyright (c) NALib, Tobias Stamm, Manderim GmbH
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the source-code.
//
// In case the source-code of this software is inaccessible to the end-user,
// the above copyright notice and this permission notice shall be included
// in any source-code which is dependent on this software and is accessible
// to the end-user.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.