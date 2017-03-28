
// This file is part of NALib, a collection of C and C++ source code
// intended for didactical purposes. Full license notice at the bottom.

#include "../NABuffer.h"
#include "../NABinaryData.h"
#include "../NAString.h"
#include "../NAURL.h"
#include <string.h>
#include <ctype.h>





NA_API NABool naEqualUTF8CStringLiterals(const NAUTF8Char* string1, const NAUTF8Char* string2, NAInt length, NABool casesensitive){
  if(!length){
    NAInt length1 = naStrlen(string1);
    NAInt length2 = naStrlen(string2);
    if(length1 != length2){return NA_FALSE;}
    length = length1;
  }
  if(casesensitive){
    int result = memcmp(string1, string2, (NAUInt)length);
    if(result){return NA_FALSE;}
  }else{
    while(length){
      NAUTF8Char curchar1;
      NAUTF8Char curchar2;
      if(isalpha((const char)*string1)){curchar1 = (NAUTF8Char)tolower(*string1);}else{curchar1 = *string1;}
      if(isalpha((const char)*string2)){curchar2 = (NAUTF8Char)tolower(*string2);}else{curchar2 = *string2;}
      if(curchar1 != curchar2){return NA_FALSE;}
      string1++;
      string2++;
      length--;
    }
  }
  return NA_TRUE;
}





struct NAString{
  NABuffer* buffer;
  NAUTF8Char* cachedstr;
};


NA_HDEF void naDestructString(NAString* string);
NA_RUNTIME_TYPE(NAString, naDestructString);




// This is the destructor for a string. It is marked as a helper as it should
// only be called by the runtime system
NA_HDEF void naDestructString(NAString* string){
  naFree(string->cachedstr);
  naReleaseBuffer(string->buffer);
}





// We especially inline this definition as it is used many times in this file.
NA_DEF NAString* naNewString(void){
  NAString* string = naNew(NAString);
  string->buffer = naCreateBuffer(NA_FALSE);
  string->cachedstr = NA_NULL;
  return string;
}




NA_DEF NAString* naNewStringWithUTF8CStringLiteral(const NAUTF8Char* ptr){
  NAString* string;
  NAInt length;
  
  #ifndef NDEBUG
    if(!ptr){
      naCrash("naNewStringWithUTF8CStringLiteral", "ptr is Null-Pointer");
      return NA_NULL;
    }
  #endif
  
  length = (NAInt)naStrlen(ptr);
  if(length){
    string = naNew(NAString);
    // C-Strings are always expected to be Null-terminated, meaning: The Byte
    // with index [size] must be binary zero. As we are not copying but just
    // referencing the pointer, we can safely use the array without this byte
    // and still be able to say: We are null-terminated!
    string->buffer = naCreateBufferConstData(ptr, length);
    string->cachedstr = NA_NULL;
    
  }else{
    string = naNewString();
  }
  
  return string;
}



NA_DEF NAString* naNewStringWithMutableUTF8Buffer(NAUTF8Char* buffer, NAInt length, NAMemoryCleanup cleanup){
  NAString* string;
  #ifndef NDEBUG
    if(!naIsCleanupValid(cleanup))
      naError("naNewStringWithMutableUTF8Buffer", "invalid cleanup option");
  #endif
  string = naNew(NAString);
  string->buffer = naCreateBufferMutableData(buffer, naAbsi(length), cleanup); // todo: absi
  string->cachedstr = NA_NULL;
  return string;
}



NA_DEF NAString* naNewStringWithFormat(const NAUTF8Char* format, ...){
  NAString* string;
  va_list argumentlist;
  va_start(argumentlist, format);
  string = naNewStringWithArguments(format, argumentlist);
  va_end(argumentlist);
  return string;
}



NA_DEF NAString* naNewStringWithArguments(const NAUTF8Char* format, va_list argumentlist){
  NAString* string;
  NAInt stringlen;
  va_list argumentlist2;
  va_list argumentlist3;
  va_copy(argumentlist2, argumentlist);
  va_copy(argumentlist3, argumentlist);
  stringlen = naVarargStringLength(format, argumentlist2);
  if(stringlen){
    NAUTF8Char* stringbuf = naMalloc(-(NAInt)stringlen);
    naVsnprintf(stringbuf, (NAUInt)(stringlen + 1), format, argumentlist3);
    string = naNewStringWithMutableUTF8Buffer(stringbuf, -(NAInt)stringlen, NA_MEMORY_CLEANUP_NA_FREE);
  }else{
    string = naNewString();
  }
  va_end(argumentlist2);
  va_end(argumentlist3);
  return string;
}



NA_DEF NAString* naNewStringExtraction(const NAString* srcstring, NAInt charoffset, NAInt length){
  NAString* string;

  #ifndef NDEBUG
    if(!srcstring){
      naCrash("naNewStringExtraction", "srcstring is Null-Pointer.");
      return NA_NULL;
    }
  #endif

  string = naNewString();

  if(naIsStringEmpty(srcstring)){
    #ifndef NDEBUG
      // If src is empty, return an empty string. But check if this is
      // intentional or an error:
      if(charoffset || length)
        naError("naNewStringExtraction", "Invalid string extraction of empty string.");
    #endif
  }else{
    // Extract the string
    NAInt positiveoffset;
    NAInt positivecount;
    naMakeIntegerRangePositiveInLength(&positiveoffset, &positivecount, charoffset, length, naGetStringBytesize(srcstring));

    string->buffer = naCreateBufferExtraction(srcstring->buffer, naMakeRangei(positiveoffset, positivecount));
    string->cachedstr = NA_NULL;
  }
  
  return string;
}



NA_DEF NAString* naNewStringWithBufferExtraction(NABuffer* buffer, NARangei range){
  NAString* string = naNew(NAString);
  string->buffer = naCreateBufferExtraction(buffer, range);
  string->cachedstr = NA_NULL;
  return string;
}



NA_DEF NAInt naGetStringBytesize(const NAString* string){
  return naGetBufferRange(string->buffer).length;
}



NA_DEF const NAUTF8Char* naGetStringUTF8Pointer(const NAString* string){
  #ifndef NDEBUG
    if(!string){
      naCrash("naGetStringUTF8Pointer", "string is Null-Pointer.");
      return NA_NULL;
    }
  #endif
  
  NAString* mutablestring = (NAString*)string;
  
  if(naIsStringEmpty(string)){
    return (const NAUTF8Char*)"";
  }else{
    naFree(mutablestring->cachedstr);
    NAInt strlen = naGetBufferRange(string->buffer).length;
    mutablestring->cachedstr = naMalloc(strlen + 1);
    naCacheBufferRange(string->buffer, naGetBufferRange(string->buffer), NA_FALSE);
    naWriteBufferToData(string->buffer, mutablestring->cachedstr);
    mutablestring->cachedstr[strlen] = '\0';
    return string->cachedstr;    
  }
}



NA_DEF NABool naIsStringEmpty(const NAString* string){
  #ifndef NDEBUG
    if(!string){
      naCrash("naIsStringEmpty", "string is Null-Pointer.");
      return NA_TRUE;
    }
  #endif
  return naIsBufferEmpty(string->buffer);
}




NA_API const NABuffer* naGetStringBufferConst(const NAString* string){
  return string->buffer;
}
NA_API NABuffer* naGetStringBufferMutable(NAString* string){
  return string->buffer;
}



NA_DEF NAString* naNewStringWithBasenameOfFilename(const NAString* filename){
  NAInt dotoffset = naSearchBufferByteOffset(filename->buffer, NA_SUFFIX_DELIMITER, naGetRangeiMax(naGetBufferRange(filename->buffer)), NA_FALSE);
  // If dotpos is invalid, return the full string.
  if(dotoffset == NA_INVALID_MEMORY_INDEX){
    return naNewStringExtraction(filename, 0, -1);
  }else{
    return naNewStringExtraction(filename, 0, dotoffset);
  }
}



NA_DEF NAString* naNewStringWithSuffixOfFilename(const NAString* filename){
  NAInt dotoffset = naSearchBufferByteOffset(filename->buffer, NA_SUFFIX_DELIMITER, naGetRangeiMax(naGetBufferRange(filename->buffer)), NA_FALSE);
  if(dotoffset == NA_INVALID_MEMORY_INDEX){
    return naNewString();
  }else{
    return naNewStringExtraction(filename, dotoffset + 1, -1);
  }
}



//NA_DEF NAString* naNewStringXMLEncoded(const NAString* inputstring){
//  NAInt i;
//  NAInt inputsize;
//  NAInt destsize;
//  const NAUTF8Char* inptr;
//  NAUTF8Char* destptr;
//  NAUTF8Char* stringbuf;
//
//  #ifndef NDEBUG
//    if(!inputstring){
//      naCrash("naNewStringXMLEncoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naNewString();}
//  
//  // Count the required number of utf8 characters.
//  destsize = 0;
//  inptr = naGetStringUTF8Pointer(inputstring);
//  for(i=0; i<inputsize; i++){
//    switch(*inptr){
//    case '&': destsize += 5; break;
//    case '<': destsize += 4; break;
//    case '>': destsize += 4; break;
//    case '\"': destsize += 6; break;
//    case '\'': destsize += 6; break;
//    default: destsize += 1; break;
//    }
//    inptr++;
//  }
//
//  #ifndef NDEBUG
//    if(destsize <= 0)
//      naError("naNewStringXMLEncoded", "encoded size invalid. String too long?");
//  #endif
//  // Create the string with the required length
//  stringbuf = naMalloc(-destsize);
//  inptr = naGetStringUTF8Pointer(inputstring);
//  destptr = stringbuf;
//
//  // Copy all characters and encode them if necessary.
//  for(i=0; i<inputsize; i++){
//    switch(*inptr){
//    case '&':  *destptr++ = '&'; *destptr++ = 'a'; *destptr++ = 'm'; *destptr++ = 'p'; *destptr++ = ';'; break;
//    case '<':  *destptr++ = '&'; *destptr++ = 'l'; *destptr++ = 't'; *destptr++ = ';'; break;
//    case '>':  *destptr++ = '&'; *destptr++ = 'g'; *destptr++ = 't'; *destptr++ = ';'; break;
//    case '\"': *destptr++ = '&'; *destptr++ = 'q'; *destptr++ = 'u'; *destptr++ = 'o'; *destptr++ = 't'; *destptr++ = ';'; break;
//    case '\'': *destptr++ = '&'; *destptr++ = 'a'; *destptr++ = 'p'; *destptr++ = 'o'; *destptr++ = 's'; *destptr++ = ';'; break;
//    default: *destptr++ = *inptr; break;
//    }
//    inptr++;
//  }
//
//  return naNewStringWithMutableUTF8Buffer(stringbuf, -destsize, NA_MEMORY_CLEANUP_NA_FREE);
//}




//NA_DEF NAString* naNewStringXMLDecoded(const NAString* inputstring){
//  NAInt inputsize;
//  const NAUTF8Char* inptr;
//  NAUTF8Char* destptr;
//  NAInt i;
//  NAInt finalsize;
//  NAUTF8Char* stringbuf;
//  NAString* string;
//
//  #ifndef NDEBUG
//    if(!inputstring){
//      naCrash("naNewStringXMLDecoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naNewString();}
//
//  // Create a string with sufficient characters. As XML entities are always
//  // longer than their decoded character, we just use the same size.
//  stringbuf = naMalloc(-inputsize);
//  string = naNewStringWithMutableUTF8Buffer(stringbuf, -inputsize, NA_MEMORY_CLEANUP_NA_FREE);
//  inptr = naGetStringUTF8Pointer(inputstring);
//  destptr = stringbuf;
//
//  // Copy all characters and decode them if necessary.
//  for(i=0; i<inputsize; i++){
//    if(inptr[i] == '&'){
//      if(((inputsize - i) >= 5) && (inptr[i+1] == 'a') && (inptr[i+2] == 'm') && (inptr[i+3] == 'p') && (inptr[i+4] == ';')){ *destptr++ = '&'; i += 4; }
//      else if(((inputsize - i) >= 4) && (inptr[i+1] == 'l') && (inptr[i+2] == 't') && (inptr[i+3] == ';')){ *destptr++ = '<'; i += 3; }
//      else if(((inputsize - i) >= 4) && (inptr[i+1] == 'g') && (inptr[i+2] == 't') && (inptr[i+3] == ';')){ *destptr++ = '>'; i += 3; }
//      else if(((inputsize - i) >= 6) && (inptr[i+1] == 'q') && (inptr[i+2] == 'u') && (inptr[i+3] == 'o') && (inptr[i+4] == 't') && (inptr[i+5] == ';')){ *destptr++ = '\"'; i += 5; }
//      else if(((inputsize - i) >= 6) && (inptr[i+1] == 'a') && (inptr[i+2] == 'p') && (inptr[i+3] == 'o') && (inptr[i+4] == 's') && (inptr[i+5] == ';')){ *destptr++ = '\''; i += 5; }
//      else{
//        *destptr++ = inptr[i];
//      }
//    }else{
//      *destptr++ = inptr[i];
//    }
//  }
//
//  // todo: numeric entities.
//
//  // The string is marked as NULL-Terminated. So we make sure this is the case.
//  *destptr = '\0';
//  // Finally, we shrink the string to its actual size
//  finalsize = destptr - stringbuf;
//  string = naNewStringExtraction(string, 0, finalsize);
//
//  return string;
//}



//NA_DEF NAString* naNewStringEPSEncoded(const NAString* inputstring){
//  NAUInt i;
//  NAUInt inputsize;
//  NAInt destsize;
//  const NAUTF8Char* inptr;
//  NAUTF8Char* destptr;
//  NAUTF8Char* stringbuf;
//  NAString* string;
//
//  #ifndef NDEBUG
//    if(!inputstring){
//      naCrash("naNewStringEPSEncoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naNewString();}
//  
//  // Count the required number of utf8 characters.
//  destsize = 0;
//  inptr = naGetStringUTF8Pointer(inputstring);
//  for(i=0; i<inputsize; i++){
//    switch(*inptr){
//    case '\\': destsize += 2; break;
//    case '(': destsize += 2; break;
//    case ')': destsize += 2; break;
//    default: destsize += 1; break;
//    }
//    inptr++;
//  }
//
//  #ifndef NDEBUG
//    if(destsize <= 0)
//      naError("naNewStringEPSEncoded", "encoded size invalid. String too long?");
//  #endif
//  // Create the string with the required length
//  stringbuf = naMalloc(-destsize);
//  string = naNewStringWithMutableUTF8Buffer(stringbuf, -destsize, NA_MEMORY_CLEANUP_NA_FREE);
//  inptr = naGetStringUTF8Pointer(inputstring);
//  destptr = stringbuf;
//
//  // Copy all characters and encode them if necessary.
//  for(i=0; i<inputsize; i++){
//    switch(*inptr){
//    case '\\': *destptr++ = '\\'; *destptr++ = '\\'; break;
//    case '(':  *destptr++ = '\\'; *destptr++ = '(';  break;
//    case ')':  *destptr++ = '\\'; *destptr++ = ')';  break;
//    default: *destptr++ = *inptr; break;
//    }
//    inptr++;
//  }
//
//  return string;
//}


//NA_DEF NAString* naNewStringEPSDecoded(const NAString* inputstring){
//
//  NAInt i;
//  NAInt inputsize;
//  const NAUTF8Char* inptr;
//  NAUTF8Char* destptr;
//  NAInt finalsize;
//  NAUTF8Char* stringbuf;
//  NAString* string;
//
//  #ifndef NDEBUG
//    if(!inputstring){
//      naCrash("naNewStringEPSDecoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naNewString();}
//
//  // Create a string with sufficient characters. As EPS entities are always
//  // longer than their decoded character, we just use the same size.
//  stringbuf = naMalloc(-inputsize);
//  string = naNewStringWithMutableUTF8Buffer(stringbuf, -inputsize, NA_MEMORY_CLEANUP_NA_FREE);
//  inptr = naGetStringUTF8Pointer(inputstring);
//  destptr = stringbuf;
//
//  // Copy all characters and decode them if necessary.
//  for(i=0; i<inputsize; i++){
//    if(inptr[i] == '\\'){
//      if(((inputsize - i) >= 2) && (inptr[i+1] == '\\')){     *destptr++ = '\\'; i += 1; }
//      else if(((inputsize - i) >= 2) && (inptr[i+1] == '(')){ *destptr++ = '(';  i += 1; }
//      else if(((inputsize - i) >= 2) && (inptr[i+1] == ')')){ *destptr++ = ')';  i += 1; }
//      else{
//        *destptr++ = inptr[i];
//      }
//    }else{
//      *destptr++ = inptr[i];
//    }
//  }
//
//  // The string is marked as NULL-Terminated. So we make sure this is the case.
//  *destptr = '\0';
//  // Finally, we shrink the string to its actual size
//  finalsize = destptr - stringbuf;
//  string = naNewStringExtraction(string, 0, finalsize);
//
//  return string;
//}



//#if NA_SYSTEM == NA_SYSTEM_WINDOWS
////  NA_DEF SystemChar* naAllocSystemStringFromString(const NAUTF8Char* utf8string, NAUInt length){
////    SystemChar* outstr;
////    NAUInt newsize;
////    if(!size){size = naStrlen(utf8string);}
////    #ifdef UNICODE
////      newsize = MultiByteToWideChar(CP_UTF8, 0, utf8string, size, NULL, 0);
////      outstr = (SystemChar*)naMalloc((newsize + 1 * sizeof(SystemChar)));
////      MultiByteToWideChar(CP_UTF8, 0, utf8string, size, outstr, newsize);
////    #else
////      newsize = size;
////      outstr = (SystemChar*)naMalloc((newsize + 1) * sizeof(SystemChar));
////      naCopyn(outstr, utf8string, newsize);
////    #endif
////    outstr[newsize] = 0;
////    return outstr;
////  }
//
//
////  NA_DEF NAString* naNewStringFromSystemString( SystemChar* systemstring){
////    NAInt newsize;
////    NAUTF8Char* stringbuf;
////    #ifdef UNICODE
////      newsize = WideCharToMultiByte(CP_UTF8, 0, systemstring, -1, NULL, 0, NULL, NULL);
////      stringbuf = naMalloc(-newsize);
////      string = naNewStringWithMutableUTF8Buffer(string, stringbuf, -newsize, NA_MEMORY_CLEANUP_NA_FREE);
////      WideCharToMultiByte(CP_UTF8, 0, systemstring, -1, stringbuf, newsize, NULL, NULL);
////    #else
////      newsize = naStrlen(systemstring);
////      stringbuf = naMalloc(-newsize);
////      string = naNewStringWithMutableUTF8Buffer(string, stringbuf, -newsize, NA_MEMORY_CLEANUP_NA_FREE);
////      naCopyn(stringbuf, systemstring, newsize);
////    #endif
////    return string;
////  }
//#endif


//
//NA_DEF void naAppendStringString(NAString* originalstring, const NAString* string2){
//  NA_UNUSED(originalstring);
//  NA_UNUSED(string2);
////  NAUInt stringsize1 = naGetStringBytesize(originalstring);
////  NAUInt stringsize2 = naGetStringBytesize(string2);
////  NAInt totalstringsize = stringsize1 + stringsize2;
////  if(totalstringsize){
////    NAUTF8Char* stringbuf = naMalloc(-totalstringsize);
////    if(stringsize1){naCopyn(stringbuf, naGetByteArrayConstPointer(&(originalstring->array)), stringsize1);}
////    if(stringsize2){naCopyn(&(stringbuf[stringsize1]), naGetByteArrayConstPointer(&(string2->array)), stringsize2);}
////    naClearByteArray(&(originalstring->array));
////    naInitByteArrayWithMutableBuffer(&(originalstring->array), stringbuf, -totalstringsize, NA_MEMORY_CLEANUP_NA_FREE);
////  }else{
////    // The string was empty and remains empty. Nothing to be done here.
////  }
//}
//
//
//
//NA_DEF void naAppendStringChar(NAString* originalstring, NAUTF8Char newchar){
//  NA_UNUSED(originalstring);
//  NA_UNUSED(newchar);
////  NAUInt stringsize = naGetStringBytesize(originalstring);
////  NAInt totalstringsize = stringsize + 1;
////  NAUTF8Char* stringbuf = naMalloc(-totalstringsize);
////  if(stringsize){naCopyn(stringbuf, naGetByteArrayConstPointer(&(originalstring->array)), stringsize);}
////  stringbuf[stringsize] = newchar;
////  naClearByteArray(&(originalstring->array));
////  naInitByteArrayWithMutableBuffer(&(originalstring->array), stringbuf, -totalstringsize, NA_MEMORY_CLEANUP_NA_FREE);
//}
//
//
//
//NA_DEF void naAppendStringFormat(NAString* originalstring, const NAUTF8Char* format, ...){
//  va_list argumentlist;
//  va_start(argumentlist, format);
//  naAppendStringArguments(originalstring, format, argumentlist);
//  va_end(argumentlist);
//}
//
//
//NA_DEF void naAppendStringArguments(NAString* originalstring, const NAUTF8Char* format, va_list argumentlist){
//  NA_UNUSED(originalstring);
//  NA_UNUSED(format);
//  NA_UNUSED(argumentlist);
////  NAUInt stringsize1;
////  NAUInt stringsize2;
////  NAInt totalstringsize;
////  va_list argumentlist2;
////  NAUTF8Char* stringbuf;
////
////  #ifndef NDEBUG
////    if(!originalstring){
////      naCrash("naAppendStringArguments", "string is Null-Pointer.");
////      return;
////    }
////  #endif
////  
////  stringsize1 = naGetStringBytesize(originalstring);
////  va_copy(argumentlist2, argumentlist);
////  stringsize2 = naVarargStringLength(format, argumentlist2);
////  va_end(argumentlist2);
////  totalstringsize = stringsize1 + stringsize2;
////  if(totalstringsize){
////    stringbuf = naMalloc(-totalstringsize);
////    if(stringsize1){naCopyn(stringbuf, naGetByteArrayConstPointer(&(originalstring->array)), stringsize1);}
////    va_copy(argumentlist2, argumentlist);
////    naVsnprintf(&(stringbuf[stringsize1]), stringsize2 + 1, format, argumentlist2);
////    va_end(argumentlist2);
////    naClearByteArray(&(originalstring->array));
////    naInitByteArrayWithMutableBuffer(&(originalstring->array), stringbuf, -totalstringsize, NA_MEMORY_CLEANUP_NA_FREE);
////  }else{
////    // The string was empty and remains empty. Nothing to be done here.
////  }
//}
//









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
