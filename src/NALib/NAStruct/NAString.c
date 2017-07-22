
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







// We especially inline this definition as it is used many times in this file.
NA_DEF NAString naMakeString(void){
  NAString string;
  string.buffer = naCreateBuffer(NA_FALSE);
  string.cachedstr = NA_NULL;
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}




NA_DEF NAString naMakeStringWithUTF8CStringLiteral(const NAUTF8Char* ptr){
  NAString string;
  NAInt length;
  
  #ifndef NDEBUG
    if(!ptr){
      naCrash("naMakeStringWithUTF8CStringLiteral", "ptr is Null-Pointer");
      return naMakeString();
    }
  #endif
  
  length = (NAInt)naStrlen(ptr);
  if(length){
    // C-Strings are always expected to be Null-terminated, meaning: The Byte
    // with index [size] must be binary zero. As we are not copying but just
    // referencing the pointer, we can safely use the array without this byte
    // and still be able to say: We are null-terminated!
    string.buffer = naCreateBufferWithConstData(ptr, length);
    string.cachedstr = NA_NULL;
    
  }else{
    string = naMakeString();
  }
  
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF NAString naMakeStringWithMutableUTF8Buffer(NAUTF8Char* buffer, NAInt length, NAMutator destructor){
  NAString string;
  string.buffer = naCreateBufferWithMutableData(buffer, naAbsi(length), destructor); // todo: absi
  string.cachedstr = NA_NULL;
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF NAString naMakeStringWithFormat(const NAUTF8Char* format, ...){
  NAString string;
  va_list argumentlist;
  va_start(argumentlist, format);
  string = naMakeStringWithArguments(format, argumentlist);
  va_end(argumentlist);
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF NAString naMakeStringWithArguments(const NAUTF8Char* format, va_list argumentlist){
  NAString string;
  NAInt stringlen;
  va_list argumentlist2;
  va_list argumentlist3;
  va_copy(argumentlist2, argumentlist);
  va_copy(argumentlist3, argumentlist);
  stringlen = naVarargStringLength(format, argumentlist2);
  if(stringlen){
    NAUTF8Char* stringbuf = naMalloc(-(NAInt)stringlen);
    naVsnprintf(stringbuf, (NAUInt)(stringlen + 1), format, argumentlist3);
    string = naMakeStringWithMutableUTF8Buffer(stringbuf, -(NAInt)stringlen, (NAMutator)naFree);
  }else{
    string = naMakeString();
  }
  va_end(argumentlist2);
  va_end(argumentlist3);
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF NAString naMakeStringExtraction(const NAString* srcstring, NAInt charoffset, NAInt length){
  NAString string;

  #ifndef NDEBUG
    if(!srcstring){
      naCrash("naMakeStringExtraction", "srcstring is Null-Pointer.");
      return naMakeString();
    }
  #endif

  if(naIsStringEmpty(srcstring)){
    #ifndef NDEBUG
      // If src is empty, return an empty string. But check if this is
      // intentional or an error:
      if(charoffset || length)
        naError("naMakeStringExtraction", "Invalid string extraction of empty string.");
    #endif
    string = naMakeString();
  }else{
    // Extract the string
    NAInt positiveoffset;
    NAInt positivecount;
    naMakeIntegerRangePositiveInLength(&positiveoffset, &positivecount, charoffset, length, naGetStringBytesize(srcstring));

    string.buffer = naCreateBufferExtraction(srcstring->buffer, naMakeRangei(positiveoffset, positivecount));
    string.cachedstr = NA_NULL;
  }
  
  naCacheBufferRange(string.buffer, naGetBufferRange(string.buffer), NA_FALSE);
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF NAString naMakeStringWithBufferExtraction(NABuffer* buffer, NARangei range){
  NAString string;
  string.buffer = naCreateBufferExtraction(buffer, range);
  string.cachedstr = NA_NULL;
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF void naClearString(NAString* string){
  naReleaseBuffer(string->buffer);
  naFree(string->cachedstr);
}



NA_DEF NAInt naGetStringBytesize(const NAString* string){
  return naGetBufferRange(string->buffer).length;
}


NA_DEF const NAUTF8Char* naGetStringUTF8Pointer(const NAString* string){
  NAString* mutablestring = (NAString*)string;

  #ifndef NDEBUG
    if(!string){
      naCrash("naGetStringUTF8Pointer", "string is Null-Pointer.");
      return NA_NULL;
    }
  #endif
    
  if(naIsStringEmpty(string)){
    return (const NAUTF8Char*)"";
  }else{
    NAInt strlen;
    naFree(mutablestring->cachedstr);
    strlen = naGetBufferRange(string->buffer).length;
    #ifndef NDEBUG
      if(!strlen)
        naError("naGetStringUTF8Pointer", "String is empty");
    #endif
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



NA_DEF NAUTF8Char naGetStringChar(NAString* string, NAInt indx){
  return (NAUTF8Char)naGetBufferByteAtIndex(string->buffer, indx);
}



NA_DEF NAString naMakeStringWithBasenameOfFilename(const NAString* filename){
  NAString string;
  NAInt dotoffset = naSearchBufferByteOffset(filename->buffer, NA_SUFFIX_DELIMITER, naGetRangeiMax(naGetBufferRange(filename->buffer)), NA_FALSE);
  // If dotpos is invalid, return the full string.
  if(dotoffset == NA_INVALID_MEMORY_INDEX){
    string = naMakeStringExtraction(filename, 0, -1);
  }else{
    string = naMakeStringExtraction(filename, 0, dotoffset);
  }
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF NAString naMakeStringWithSuffixOfFilename(const NAString* filename){
  NAString string;
  NAInt dotoffset = naSearchBufferByteOffset(filename->buffer, NA_SUFFIX_DELIMITER, naGetRangeiMax(naGetBufferRange(filename->buffer)), NA_FALSE);
  if(dotoffset == NA_INVALID_MEMORY_INDEX){
    string = naMakeString();
  }else{
    string = naMakeStringExtraction(filename, dotoffset + 1, -1);
  }
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(&string);
  #endif
  return string;
}



NA_DEF NAString naMakeStringCEscaped  (const NAString* inputstring){
  NAUTF8Char outbuffer[10]; // this is the maximal number of chars added.
  NAString string;
  NABuffer* buffer = naCreateBuffer(NA_FALSE);
  NABufferIterator iter = naMakeBufferAccessor(inputstring->buffer);
  NABufferIterator outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtEnd(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    switch(curchar){
    case '\a': outbuffer[0] = '\\'; outbuffer[1] = 'a',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\b': outbuffer[0] = '\\'; outbuffer[1] = 'b',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\f': outbuffer[0] = '\\'; outbuffer[1] = 'f',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\n': outbuffer[0] = '\\'; outbuffer[1] = 'n',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\r': outbuffer[0] = '\\'; outbuffer[1] = 'r',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\t': outbuffer[0] = '\\'; outbuffer[1] = 't',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\v': outbuffer[0] = '\\'; outbuffer[1] = 'v',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\\': outbuffer[0] = '\\'; outbuffer[1] = '\\', naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\'': outbuffer[0] = '\\'; outbuffer[1] = '\'', naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\"': outbuffer[0] = '\\'; outbuffer[1] = '\"', naWriteBufferBytes(&outiter, outbuffer, 2); break;
    case '\?': outbuffer[0] = '\\'; outbuffer[1] = '?',  naWriteBufferBytes(&outiter, outbuffer, 2); break;
    default: naWriteBufferi8(&outiter, curchar); break;
    }
  }
  naClearBufferIterator(&outiter);
  naClearBufferIterator(&iter);
  string = naMakeStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naReleaseBuffer(buffer);
  return string;
}



NA_DEF NAString naMakeStringCUnescaped(const NAString* inputstring){
  NAString string;
  NABuffer* buffer = naCreateBuffer(NA_FALSE);
  NABufferIterator iter = naMakeBufferAccessor(inputstring->buffer);
  NABufferIterator outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtEnd(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    if(curchar == '\\'){
      curchar = naReadBufferi8(&iter);
      switch(curchar){
      case 'a':  naWriteBufferi8(&outiter, 'a');  break;
      case 'b':  naWriteBufferi8(&outiter, 'b');  break;
      case 'f':  naWriteBufferi8(&outiter, 'f');  break;
      case 'n':  naWriteBufferi8(&outiter, 'n');  break;
      case 'r':  naWriteBufferi8(&outiter, 'r');  break;
      case 't':  naWriteBufferi8(&outiter, 't');  break;
      case 'v':  naWriteBufferi8(&outiter, 'v');  break;
      case '\\': naWriteBufferi8(&outiter, '\\'); break;
      case '\'': naWriteBufferi8(&outiter, '\''); break;
      case '\"': naWriteBufferi8(&outiter, '\"'); break;
      case '?':  naWriteBufferi8(&outiter, '?');  break;
    // todo: Add more decodings
      default:
        #ifndef NDEBUG
          naError("naMakeStringCUnescaped", "Unrecognized escape character");
        #endif
        naWriteBufferi8(&outiter, curchar);
        break;
      }
    }else{
      naWriteBufferi8(&outiter, curchar);
    }
  }
  naClearBufferIterator(&outiter);
  naClearBufferIterator(&iter);
  string = naMakeStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naReleaseBuffer(buffer);
  return string;
}



NA_DEF NAString naMakeStringXMLEncoded(const NAString* inputstring){
  NA_UNUSED(inputstring);
//  NAInt i;
//  NAInt inputsize;
//  NAInt destsize;
//  const NAUTF8Char* inptr;
//  NAUTF8Char* destptr;
//  NAUTF8Char* stringbuf;
//
//  #ifndef NDEBUG
//    if(!inputstring){
//      naCrash("naMakeStringXMLEncoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naMakeString();}
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
//      naError("naMakeStringXMLEncoded", "encoded size invalid. String too long?");
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
//  return naMakeStringWithMutableUTF8Buffer(stringbuf, -destsize, naFree);
  return naMakeString();
}




NA_DEF NAString naMakeStringXMLDecoded(const NAString* inputstring){
  NA_UNUSED(inputstring);
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
//      naCrash("naMakeStringXMLDecoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naMakeString();}
//
//  // Create a string with sufficient characters. As XML entities are always
//  // longer than their decoded character, we just use the same size.
//  stringbuf = naMalloc(-inputsize);
//  string = naMakeStringWithMutableUTF8Buffer(stringbuf, -inputsize, naFree);
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
//  string = naMakeStringExtraction(string, 0, finalsize);
//
//  return string;
  return naMakeString();
}



NA_DEF NAString naMakeStringEPSEncoded(const NAString* inputstring){
  NA_UNUSED(inputstring);
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
//      naCrash("naMakeStringEPSEncoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naMakeString();}
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
//      naError("naMakeStringEPSEncoded", "encoded size invalid. String too long?");
//  #endif
//  // Create the string with the required length
//  stringbuf = naMalloc(-destsize);
//  string = naMakeStringWithMutableUTF8Buffer(stringbuf, -destsize, naFree);
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
  return naMakeString();
}


NA_DEF NAString naMakeStringEPSDecoded(const NAString* inputstring){
  NA_UNUSED(inputstring);
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
//      naCrash("naMakeStringEPSDecoded", "input string is Null-Pointer.");
//      return NA_NULL;
//    }
//  #endif
//
//  inputsize = naGetStringBytesize(inputstring);
//  if(!inputsize){return naMakeString();}
//
//  // Create a string with sufficient characters. As EPS entities are always
//  // longer than their decoded character, we just use the same size.
//  stringbuf = naMalloc(-inputsize);
//  string = naMakeStringWithMutableUTF8Buffer(stringbuf, -inputsize, naFree);
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
//  string = naMakeStringExtraction(string, 0, finalsize);
//
//  return string;
  return naMakeString();
}



#if NA_SYSTEM == NA_SYSTEM_WINDOWS
  NA_DEF SystemChar* naAllocSystemStringWithUTF8String(const NAUTF8Char* utf8string, NAUInt length){
    SystemChar* outstr;
    NAUInt newlength;
    if(!length){length = naStrlen(utf8string);}
    #ifdef UNICODE
      newsize = MultiByteToWideChar(CP_UTF8, 0, utf8string, size, NULL, 0);
      outstr = (SystemChar*)naMalloc((newsize + 1 * sizeof(SystemChar)));
      MultiByteToWideChar(CP_UTF8, 0, utf8string, size, outstr, newsize);
    #else
      newlength = length;
      outstr = (SystemChar*)naMalloc((newlength + 1) * sizeof(SystemChar));
      naCopyn(outstr, utf8string, newlength);
    #endif
    outstr[newlength] = 0;
    return outstr;
  }


  //NA_DEF NAString* naMakeStringFromSystemString( SystemChar* systemstring){
  //  NAInt newsize;
  //  NAUTF8Char* stringbuf;
  //  #ifdef UNICODE
  //    newsize = WideCharToMultiByte(CP_UTF8, 0, systemstring, -1, NULL, 0, NULL, NULL);
  //    stringbuf = naMalloc(-newsize);
  //    string = naMakeStringWithMutableUTF8Buffer(string, stringbuf, -newsize, naFree);
  //    WideCharToMultiByte(CP_UTF8, 0, systemstring, -1, stringbuf, newsize, NULL, NULL);
  //  #else
  //    newsize = naStrlen(systemstring);
  //    stringbuf = naMalloc(-newsize);
  //    string = naMakeStringWithMutableUTF8Buffer(string, stringbuf, -newsize, naFree);
  //    naCopyn(stringbuf, systemstring, newsize);
  //  #endif
  //  return string;
  //}
#endif



NA_DEF void naAppendStringString(NAString* originalstring, const NAString* string2){
  naAppendBufferToBuffer(originalstring->buffer, string2->buffer);
}



NA_DEF void naAppendStringChar(NAString* originalstring, NAUTF8Char newchar){
  NAString charstring = naMakeStringWithFormat("%c", newchar);
  naAppendBufferToBuffer(originalstring->buffer, charstring.buffer);
  naClearString(&charstring);
}



NA_DEF void naAppendStringFormat(NAString* originalstring, const NAUTF8Char* format, ...){
  va_list argumentlist;
  va_start(argumentlist, format);
  naAppendStringArguments(originalstring, format, argumentlist);
  va_end(argumentlist);
}


NA_DEF void naAppendStringArguments(NAString* originalstring, const NAUTF8Char* format, va_list argumentlist){
  NAString formatstring = naMakeStringWithArguments(format, argumentlist);
  naAppendBufferToBuffer(originalstring->buffer, formatstring.buffer);
  naClearString(&formatstring);
}





NA_DEF NABool naEqualStringToString(const NAString* string1, const NAString* string2, NABool casesensitive){
  return naEqualBufferToBuffer(string1->buffer, string2->buffer, casesensitive);
}



NA_DEF NABool naEqualStringToUTF8CStringLiteral(const NAString* string1, const NAUTF8Char* string2, NABool casesensitive){
  return naEqualBufferToData(string1->buffer, string2, naStrlen(string2), casesensitive);
}



NA_DEF int8 naParseStringInt8(const NAString* string){
  int8 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt8(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF int16 naParseStringInt16(const NAString* string){
  int16 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt16(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF int32 naParseStringInt32(const NAString* string){
  int32 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt32(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF int64 naParseStringInt64(const NAString* string){
  int64 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt64(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}



NA_DEF uint8 naParseStringUInt8(const NAString* string){
  uint8 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt8(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF uint16 naParseStringUInt16(const NAString* string){
  uint16 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt16(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF uint32 naParseStringUInt32(const NAString* string){
  uint32 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt32(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF uint64 naParseStringUInt64(const NAString* string){
  uint64 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naSeekBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt64(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}






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
