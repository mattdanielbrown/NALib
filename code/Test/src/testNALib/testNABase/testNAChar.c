
#include "../../testHelper.h"
#include "NABase.h"
#include <stdio.h>



void testNAChar(){
  naG("typedefs"){
    NAUTF8Char utf8Char = 'M';
    naT(utf8Char == 'M');
  }
}



void printNAChar(){
  printf("NAChar.h fixed values:" NA_NL);

  printMacroPlain(NA_NL_UNIX);
  printMacroPlain(NA_NL_MAC9);
  printMacroPlain(NA_NL_WIN);
  printMacroPlain(NA_TAB);
  printMacroPlain(NA_PATH_DELIMITER_UNIX);
  printMacroPlain(NA_PATH_DELIMITER_WIN);
  printMacroPlain(NA_SUFFIX_DELIMITER);

  printf(NA_NL "NAChar.h computed values:" NA_NL);
  printMacroPlain(NA_NL);
  printMacroString(NA_PRIi8);
  printMacroString(NA_PRIu8);
  printMacroString(NA_PRIx8);
  printMacroString(NA_SCNi8);
  printMacroString(NA_SCNu8);
  printMacroString(NA_PRIi16);
  printMacroString(NA_PRIu16);
  printMacroString(NA_PRIx16);
  printMacroString(NA_SCNi16);
  printMacroString(NA_SCNu16);
  printMacroString(NA_PRIi32);
  printMacroString(NA_PRIu32);
  printMacroString(NA_PRIx32);
  printMacroString(NA_SCNi32);
  printMacroString(NA_SCNu32);
  printMacroString(NA_PRIi64);
  printMacroString(NA_PRIu64);
  printMacroString(NA_PRIx64);
  printMacroString(NA_SCNi64);
  printMacroString(NA_SCNu64);

  printf(NA_NL);
}



// This is free and unencumbered software released into the public domain.

// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <http://unlicense.org/>
