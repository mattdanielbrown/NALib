
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.


//
// This is just a small file to get you started using NALib.
//
// This file should compile and run and print some version notes on the screen.
//

#include "NATree.h"
#include "NARandom.h"

void testTree(){
  NATreeConfiguration* config = naCreateTreeConfiguration(NA_TREE_KEY_DOUBLE);
  NATree tree;
  naInitTree(&tree, config); 
  NADateTime t1, t2;
  NATreeIterator iter;

//  double dummy1 = naUniformRandZE();
//  double dummy2 = naUniformRandZE();

  t1 = naMakeDateTimeNow();
  iter = naMakeTreeModifier(&tree);
  for(int i=0; i<1000000; i++){
    double key = naUniformRandZE();
//    double key = (double)i / 1000000.;
//    printf("%f\n", key);
    naAddTreeConst(&iter, &key, NA_NULL, NA_FALSE);
  }
  naClearTreeIterator(&iter);
  t2 = naMakeDateTimeNow();
  printf("Insert: %f\n", naGetDateTimeDifference(&t2, &t1));

  // ////////////////

//  double prevkey = -1.;
  t1 = naMakeDateTimeNow();
  iter = naMakeTreeModifier(&tree);
//  int i = 0;
  while(naIterateTree(&iter)){
//    const double* key = naGetTreeCurKey(&iter);
//    if(*key < prevkey){
//      printf("Wrong sorting: %d: %f, %f\n", i, *key, prevkey);
//    }
//    prevkey = *key;
//    i++;
//    if(i < 5){printf("%f\n", *key);}
  }
  naClearTreeIterator(&iter);
  t2 = naMakeDateTimeNow();
  printf("Iterate: %f\n", naGetDateTimeDifference(&t2, &t1));
  
  naClearTree(&tree);
  naReleaseTreeConfiguration(config);
}




#include "../NALib/NABase.h"
#include <stdio.h>

int main(void){
  printf("NALib Version: %d ", NA_VERSION);
  #ifndef NDEBUG
    printf("(Debug ");
  #else
    printf("(Release ");
  #endif
  printf("%d Bits Addresses, %d Bits Integers)\n", NA_SYSTEM_ADDRESS_BITS, NA_TYPE_NAINT_BITS);

  naStartRuntime();
    testTree();
  naStopRuntime();

  #if NA_OS == NA_OS_WINDOWS
   NA_UNUSED(getchar());
  #endif
  
  return 0;
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

