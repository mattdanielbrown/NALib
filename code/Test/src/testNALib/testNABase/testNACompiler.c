
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.

#include "../../testNALib.h"

void printNACompiler(){
  printf("NACompiler.h:" NA_NL);
  
  int def = 0;
  #if defined NA_C90
    def = 1;
  #endif
  printMacroDefined    (NA_C90, def);

  def = 0;
  #if defined NA_C95
    def = 1;
  #endif
  printMacroDefined    (NA_C95, def);

  def = 0;
  #if defined NA_C99
    def = 1;
  #endif
  printMacroDefined    (NA_C99, def);

  def = 0;
  #if defined NA_C11
    def = 1;
  #endif
  printMacroDefined    (NA_C11, def);

  def = 0;
  #if defined NA_CPP98
    def = 1;
  #endif
  printMacroDefined    (NA_CPP98, def);

  def = 0;
  #if defined NA_CPP11
    def = 1;
  #endif
  printMacroDefined    (NA_CPP11, def);

  def = 0;
  #if defined NA_CPP14
    def = 1;
  #endif
  printMacroDefined    (NA_CPP14, def);

  printf(NA_NL);
}



// Copyright (c) NALib, Tobias Stamm
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
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
