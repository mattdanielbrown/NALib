
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.

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
  #ifndef NDEBUG
    const NAUTF8Char* cachedstr;
  #endif
};

NA_HAPI void naDestructString(NAString* string);
NA_RUNTIME_TYPE(NAString, naDestructString, NA_FALSE);




// We especially inline this definition as it is used many times in this file.
NA_DEF NAString* naNewString(void){
  NAString* string = naNew(NAString);
  string->buffer = naNewBuffer(NA_FALSE);
  #ifndef NDEBUG
    string->cachedstr = NA_NULL;
  #endif
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}




NA_DEF NAString* naNewStringWithUTF8CStringLiteral(const NAUTF8Char* ptr){
  NAString* string;
  NAInt length;

  #ifndef NDEBUG
    if(!ptr)
      naCrash("ptr is Null-Pointer");
  #endif

  length = (NAInt)naStrlen(ptr);
  if(length){
    // C-Strings are always expected to be Null-terminated, meaning: The Byte
    // with index [size] must be binary zero. As we are not copying but just
    // referencing the pointer, we can safely use the array without this byte
    // and still be able to say: We are null-terminated!
    string = naNew(NAString);
    string->buffer = naNewBufferWithConstData(ptr, length);
    #ifndef NDEBUG
      string->cachedstr = NA_NULL;
    #endif

  }else{
    string = naNewString();
  }

  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}



NA_DEF NAString* naNewStringWithMutableUTF8Buffer(NAUTF8Char* buffer, NAInt length, NAMutator destructor){
  NAString* string = naNew(NAString);
  string->buffer = naNewBufferWithMutableData(buffer, length, destructor);
  #ifndef NDEBUG
    string->cachedstr = NA_NULL;
  #endif
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}



NA_DEF NAString* naNewStringWithFormat(const NAUTF8Char* format, ...){
  NAString* string;
  va_list argumentlist;
  va_start(argumentlist, format);
  string = naNewStringWithArguments(format, argumentlist);
  va_end(argumentlist);
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
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
    NAUTF8Char* stringbuf = naMalloc((NAInt)stringlen + 1);
    naVsnprintf(stringbuf, (NAUInt)(stringlen + 1), format, argumentlist3);
    stringbuf[stringlen] = '\0';
    string = naNewStringWithMutableUTF8Buffer(stringbuf, (NAInt)stringlen, (NAMutator)naFree);
  }else{
    string = naNewString();
  }
  va_end(argumentlist2);
  va_end(argumentlist3);
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}



NA_DEF NAString* naNewStringExtraction(const NAString* srcstring, NAInt offset, NAInt length){
  NAString* string = naNewString();

  #ifndef NDEBUG
    if(!srcstring)
      naCrash("srcstring is Null-Pointer.");
  #endif

  // Extract the string
  naRelease(string->buffer);
  string->buffer = naNewBufferExtraction(srcstring->buffer, offset, length);
  #ifndef NDEBUG
    string->cachedstr = NA_NULL;
  #endif

  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}



NA_DEF NAString* naNewStringWithBufferExtraction(NABuffer* buffer, NARangei range){
  NAString* string;
  #ifndef NDEBUG
    if(!naIsLengthValueUsefuli(buffer->range.length))
      naError("Buffer Range length is not useful.");
    if(!naIsLengthValueUsefuli(range.length))
      naError("Range length is not useful.");
  #endif
  string = naNew(NAString);
  string->buffer = naNewBufferExtraction(buffer, range.origin, range.length);
  #ifndef NDEBUG
    string->cachedstr = NA_NULL;
  #endif
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}



NA_API NAString* naNewStringWithNewlineSanitization( NAString* string, NANewlineEncoding encoding){
  NAString* newstring;
  if(naIsStringEmpty(string)){
    newstring = naNewString();
  }else{
    NABufferIterator readiter;
    NABufferIterator writeiter;
    NABool writeNL;
    
    NABuffer* newbuffer = naNewBuffer(NA_FALSE);
    naSetBufferNewlineEncoding(newbuffer, encoding);
    readiter = naMakeBufferAccessor(naGetStringBufferConst(string));
    writeiter = naMakeBufferModifier(newbuffer);
    writeNL = NA_FALSE;
    while(!naIsBufferAtEnd(&readiter)){
      NAString* line;
      if(writeNL){
        naWriteBufferNewLine(&writeiter);
      }else{
        writeNL = NA_TRUE;
      }
      line = naParseBufferLine(&readiter, NA_FALSE);
      naWriteBufferString(&writeiter, line);
      naDelete(line);
    }
    naClearBufferIterator(&readiter);
    naClearBufferIterator(&writeiter);
    newstring = naNewStringWithBufferExtraction(newbuffer, naGetBufferRange(newbuffer));
    naRelease(newbuffer);
  }
  return newstring;
}



NA_HDEF void naDestructString(NAString* string){
  naRelease(string->buffer);
}



NA_DEF NAInt naGetStringBytesize(const NAString* string){
  return naGetBufferRange(string->buffer).length;
}


NA_DEF const NAUTF8Char* naGetStringUTF8Pointer(const NAString* string){
  #ifndef NDEBUG
    NAString* mutablestring = (NAString*)string;
    if(!string){
      naCrash("string is Null-Pointer.");
      return NA_NULL;
    }
  #endif

  if(naIsStringEmpty(string)){
    return (const NAUTF8Char*)"";
  }else{
    NAInt strlen;
    NAUTF8Char* newstr;
    strlen = naGetBufferRange(string->buffer).length;
    #ifndef NDEBUG
      if(!strlen)
        naError("String is empty");
    #endif
    newstr = naMallocTmp((NAUInt)(strlen + 1));
    naCacheBufferRange(string->buffer, naGetBufferRange(string->buffer));
    naWriteBufferToData(string->buffer, newstr);
    newstr[strlen] = '\0';
    #ifndef NDEBUG
      mutablestring->cachedstr = newstr;
    #endif
    return newstr;
  }
}



NA_DEF NABool naIsStringEmpty(const NAString* string){
  #ifndef NDEBUG
    if(!string){
      naCrash("string is Null-Pointer.");
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



NA_DEF NAString* naNewStringWithBasenameOfFilename(const NAString* filename){
  NAString* string;
  NAInt dotoffset = naSearchBufferByteOffset(filename->buffer, NA_SUFFIX_DELIMITER, naGetRangeiMax(naGetBufferRange(filename->buffer)), NA_FALSE);
  // If dotpos is invalid, return the full string.
  if(dotoffset == NA_INVALID_MEMORY_INDEX){
    string = naNewStringExtraction(filename, 0, -1);
  }else{
    string = naNewStringExtraction(filename, 0, dotoffset);
  }
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}



NA_DEF NAString* naNewStringWithSuffixOfFilename(const NAString* filename){
  NAString* string;
  NAInt dotoffset = naSearchBufferByteOffset(filename->buffer, NA_SUFFIX_DELIMITER, naGetRangeiMax(naGetBufferRange(filename->buffer)), NA_FALSE);
  if(dotoffset == NA_INVALID_MEMORY_INDEX){
    string = naNewString();
  }else{
    string = naNewStringExtraction(filename, dotoffset + 1, -1);
  }
  #if NA_STRING_ALWAYS_CACHE == 1
    naGetStringUTF8Pointer(string);
  #endif
  return string;
}



NA_DEF NAString* naNewStringCEscaped(const NAString* inputstring){
  NAString* string;
  NABuffer* buffer;
  NABufferIterator iter;
  NABufferIterator outiter;
  if(naIsStringEmpty(inputstring)){
    return naNewString();
  }
  iter = naMakeBufferAccessor(inputstring->buffer);
  naLocateBufferStart(&iter);
  buffer = naNewBuffer(NA_FALSE);
  outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtInitial(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    switch(curchar){
    case '\a': naWriteBufferBytes(&outiter, "\\a", 2); break;
    case '\b': naWriteBufferBytes(&outiter, "\\b", 2); break;
    case '\f': naWriteBufferBytes(&outiter, "\\f", 2); break;
    case '\n': naWriteBufferBytes(&outiter, "\\n", 2); break;
    case '\r': naWriteBufferBytes(&outiter, "\\r", 2); break;
    case '\t': naWriteBufferBytes(&outiter, "\\t", 2); break;
    case '\v': naWriteBufferBytes(&outiter, "\\v", 2); break;
    case '\\': naWriteBufferBytes(&outiter, "\\\\", 2); break;
    case '\'': naWriteBufferBytes(&outiter, "\\\'", 2); break;
    case '\"': naWriteBufferBytes(&outiter, "\\\"", 2); break;
    case '\?': naWriteBufferBytes(&outiter, "\\?", 2); break;
    // todo: Add more escapes
    default: naWriteBufferi8(&outiter, curchar); break;
    }
  }
  naClearBufferIterator(&outiter);
  naClearBufferIterator(&iter);
  string = naNewStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naRelease(buffer);
  return string;
}



NA_DEF NAString* naNewStringCUnescaped(const NAString* inputstring){
  NAString* string;
  NABuffer* buffer;
  NABufferIterator iter;
  NABufferIterator outiter;
  if(naIsStringEmpty(inputstring)){
    return naNewString();
  }
  buffer = naNewBuffer(NA_FALSE);
  iter = naMakeBufferAccessor(inputstring->buffer);
  naLocateBufferStart(&iter);
  outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtInitial(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    if(curchar == '\\'){
      curchar = naReadBufferi8(&iter);
      switch(curchar){
      case 'a':  naWriteBufferi8(&outiter, '\a');  break;
      case 'b':  naWriteBufferi8(&outiter, '\b');  break;
      case 'f':  naWriteBufferi8(&outiter, '\f');  break;
      case 'n':  naWriteBufferi8(&outiter, '\n');  break;
      case 'r':  naWriteBufferi8(&outiter, '\r');  break;
      case 't':  naWriteBufferi8(&outiter, '\t');  break;
      case 'v':  naWriteBufferi8(&outiter, '\v');  break;
      case '\\': naWriteBufferi8(&outiter, '\\'); break;
      case '\'': naWriteBufferi8(&outiter, '\''); break;
      case '\"': naWriteBufferi8(&outiter, '\"'); break;
      case '?':  naWriteBufferi8(&outiter, '\?');  break;
    // todo: Add more escapes
      default:
        #ifndef NDEBUG
          naError("Unrecognized escape character");
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
  string = naNewStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naRelease(buffer);
  return string;
}



NA_DEF NAString* naNewStringXMLEncoded(const NAString* inputstring){
  NAString* string;
  NABuffer* buffer;
  NABufferIterator iter;
  NABufferIterator outiter;
  if(naIsStringEmpty(inputstring)){
    return naNewString();
  }
  buffer = naNewBuffer(NA_FALSE);
  iter = naMakeBufferAccessor(inputstring->buffer);
  outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtInitial(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    switch(curchar){
    case '&': naWriteBufferBytes(&outiter, "&amp;", 5); break;
    case '<': naWriteBufferBytes(&outiter, "&lt;", 4); break;
    case '>': naWriteBufferBytes(&outiter, "&gt;", 4); break;
    case '\"': naWriteBufferBytes(&outiter, "&quot;", 6); break;
    case '\'': naWriteBufferBytes(&outiter, "&apos;", 6); break;
    default: naWriteBufferi8(&outiter, curchar); break;
    }
  }
  naClearBufferIterator(&outiter);
  naClearBufferIterator(&iter);
  string = naNewStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naRelease(buffer);
  return string;
}




NA_DEF NAString* naNewStringXMLDecoded(const NAString* inputstring){
  NAString* string;
  NABuffer* buffer;
  NABufferIterator iter;
  NABufferIterator outiter;
  if(naIsStringEmpty(inputstring)){
    return naNewString();
  }
  buffer = naNewBuffer(NA_FALSE);
  iter = naMakeBufferAccessor(inputstring->buffer);
  outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtInitial(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    if(curchar == '&'){
      NAString* token = naParseBufferTokenWithDelimiter(&iter, ';');
      if     (naEqualStringToUTF8CStringLiteral(token, "amp",   NA_TRUE)){naWriteBufferi8(&outiter, '&');}
      else if(naEqualStringToUTF8CStringLiteral(token, "lt",    NA_TRUE)){naWriteBufferi8(&outiter, '<');}
      else if(naEqualStringToUTF8CStringLiteral(token, "gt",    NA_TRUE)){naWriteBufferi8(&outiter, '>');}
      else if(naEqualStringToUTF8CStringLiteral(token, "quot",  NA_TRUE)){naWriteBufferi8(&outiter, '\"');}
      else if(naEqualStringToUTF8CStringLiteral(token, "apos",  NA_TRUE)){naWriteBufferi8(&outiter, '\'');}
      else{
        #ifndef NDEBUG
          naError("Could not decode entity");
        #endif
        naWriteBufferi8(&outiter, curchar);
        naWriteBufferString(&outiter, token);
        naWriteBufferi8(&outiter, ';');
      }
      naDelete(token);
    }else{
      naWriteBufferi8(&outiter, curchar);
    }
  }
  naClearBufferIterator(&outiter);
  naClearBufferIterator(&iter);
  string = naNewStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naRelease(buffer);
  return string;
}



NA_DEF NAString* naNewStringEPSEncoded(const NAString* inputstring){
  NAString* string;
  NABuffer* buffer;
  NABufferIterator iter;
  NABufferIterator outiter;
  if(naIsStringEmpty(inputstring)){
    return naNewString();
  }
  buffer = naNewBuffer(NA_FALSE);
  iter = naMakeBufferAccessor(inputstring->buffer);
  outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtInitial(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    switch(curchar){
    case '\\': naWriteBufferBytes(&outiter, "\\\\", 2); break;
    case '(':  naWriteBufferBytes(&outiter, "\\(", 2); break;
    case ')':  naWriteBufferBytes(&outiter, "\\)", 2); break;
    default: naWriteBufferi8(&outiter, curchar); break;
    }
  }
  naClearBufferIterator(&outiter);
  naClearBufferIterator(&iter);
  string = naNewStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naRelease(buffer);
  return string;
}


NA_DEF NAString* naNewStringEPSDecoded(const NAString* inputstring){
  NAString* string;
  NABuffer* buffer;
  NABufferIterator iter;
  NABufferIterator outiter;
  if(naIsStringEmpty(inputstring)){
    return naNewString();
  }
  buffer = naNewBuffer(NA_FALSE);
  iter = naMakeBufferAccessor(inputstring->buffer);
  outiter = naMakeBufferModifier(buffer);
  while(!naIsBufferAtInitial(&iter)){
    NAUTF8Char curchar = naReadBufferi8(&iter);
    if(curchar == '\\'){
      curchar = naReadBufferi8(&iter);
      switch(curchar){
      case '\\':  naWriteBufferi8(&outiter, '\\');  break;
      case '(':  naWriteBufferi8(&outiter, '(');  break;
      case ')':  naWriteBufferi8(&outiter, ')');  break;
      default:
        #ifndef NDEBUG
          naError("Unrecognized escape character");
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
  string = naNewStringWithBufferExtraction(buffer, naGetBufferRange(buffer));
  naRelease(buffer);
  return string;
}



#if NA_OS == NA_OS_WINDOWS
  NA_DEF wchar_t* naAllocWideCharStringWithUTF8String(const NAUTF8Char* utf8string){
    wchar_t* outstr;
    NAUInt length;
    NAString* string = naNewStringWithUTF8CStringLiteral(utf8string);
    NAString* newlinestring = naNewStringWithNewlineSanitization(string, NA_NEWLINE_WIN);
    length = naGetStringBytesize(newlinestring);
    naDelete(string);
    NAUInt widelength;
    widelength = MultiByteToWideChar(CP_UTF8, 0, naGetStringUTF8Pointer(newlinestring), (int)length, NULL, 0);
    outstr = (wchar_t*)naMalloc(((widelength + 1) * naSizeof(wchar_t)));
    MultiByteToWideChar(CP_UTF8, 0, naGetStringUTF8Pointer(newlinestring), (int)length, outstr, (int)widelength);
    outstr[widelength] = 0;
    naDelete(newlinestring);
    return outstr;
  }

  NA_DEF char* naAllocAnsiStringWithUTF8String(const NAUTF8Char* utf8string){
    char* outstr;
    NAUInt length;
    NAString* string = naNewStringWithUTF8CStringLiteral(utf8string);
    NAString* newlinestring = naNewStringWithNewlineSanitization(string, NA_NEWLINE_WIN);
    length = naGetStringBytesize(newlinestring);
    naDelete(string);
    NAUInt widelength;
    wchar_t* wstr;
    // We have to convert UTF8 first to WideChar, then back to 8 bit Ansi.
    widelength = MultiByteToWideChar(CP_UTF8, 0, naGetStringUTF8Pointer(newlinestring), (int)length, NULL, 0);
    wstr = naMalloc(((widelength + 1) * naSizeof(wchar_t)));
    MultiByteToWideChar(CP_UTF8, 0, naGetStringUTF8Pointer(newlinestring), (int)length, wstr, (int)widelength);
    wstr[widelength] = 0;
    NAUInt ansilength = WideCharToMultiByte(CP_ACP, 0, wstr, (int)widelength, NA_NULL, 0, NA_NULL, NA_NULL);
    outstr = (char*)naMalloc(((ansilength + 1) * naSizeof(char)));
    WideCharToMultiByte(CP_ACP, 0, wstr, (int)widelength, outstr, (int)ansilength, NA_NULL, NA_NULL);
    naFree(wstr);
    outstr[ansilength] = 0;
    naDelete(newlinestring);
    return outstr;
  }

  // Returns a newly allocated memory block containing the system-encoded
  // string. If you do not provide the length, it will be automatically
  // computed. The resulting string must be freed manually. COPIES ALWAYS!
  NA_DEF TCHAR* naAllocSystemStringWithUTF8String(const NAUTF8Char* utf8string){
    #ifdef UNICODE
      return naAllocWideCharStringWithUTF8String(utf8string);
    #else
      return naAllocAnsiStringWithUTF8String(utf8string);
    #endif
  }

  NA_DEF NAString* naNewStringFromWideCharString(wchar_t* wcharstring){
    NAString* string;
    NAString* newlinestring;
    NAUInt length;
    NAInt utf8length;
    NAUTF8Char* stringbuf;
    length = wcslen(wcharstring);
    utf8length = WideCharToMultiByte(CP_UTF8, 0, wcharstring, (int)length, NULL, 0, NULL, NULL);
    stringbuf = naMalloc((utf8length + 1) * naSizeof(NAUTF8Char));
    WideCharToMultiByte(CP_UTF8, 0, wcharstring, (int)length, stringbuf, (int)utf8length, NULL, NULL);
    newlinestring = naNewStringWithMutableUTF8Buffer(stringbuf, utf8length, (NAMutator)naFree);
    string = naNewStringWithNewlineSanitization(newlinestring, NA_NEWLINE_UNIX);
    naDelete(newlinestring);
    return string;
  }

  NA_DEF NAString* naNewStringFromAnsiString(char* ansistring){
    NAString* string;
    NAString* newlinestring;
    NAUInt length;
    NAUInt widelength;
    wchar_t* wstr;
    NAUTF8Char* stringbuf;
    length = strlen(ansistring);
    widelength = MultiByteToWideChar(CP_ACP, 0, ansistring, (int)length, NULL, 0);
    wstr = naMalloc(((widelength + 1) * naSizeof(wchar_t)));
    MultiByteToWideChar(CP_ACP, 0, ansistring, (int)length, wstr, (int)widelength);
    wstr[widelength] = 0;
    NAUInt utf8length = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)widelength, NA_NULL, 0, NA_NULL, NA_NULL);
    stringbuf = (NAUTF8Char*)naMalloc(((utf8length + 1) * naSizeof(NAUTF8Char)));
    WideCharToMultiByte(CP_UTF8, 0, wstr, (int)widelength, stringbuf, (int)utf8length, NA_NULL, NA_NULL);
    naFree(wstr);
    stringbuf[utf8length] = 0;
    newlinestring = naNewStringWithMutableUTF8Buffer(stringbuf, utf8length, (NAMutator)naFree);
    string = naNewStringWithNewlineSanitization(newlinestring, NA_NEWLINE_UNIX);
    naDelete(newlinestring);
    return string;
  }

  // Creates a new NAString from a system-encoded string. COPIES ALWAYS!
  NA_DEF NAString* naNewStringFromSystemString(TCHAR* systemstring){
    #ifdef UNICODE
      return naNewStringFromWideCharString(systemstring);
    #else
      return naNewStringFromAnsiString(systemstring);
    #endif
  }
#endif



NA_DEF void naAppendStringString(NAString* originalstring, const NAString* string2){
  naAppendBufferToBuffer(originalstring->buffer, string2->buffer);
}



NA_DEF void naAppendStringChar(NAString* originalstring, NAUTF8Char newchar){
  NAString* charstring = naNewStringWithFormat("%c", newchar);
  naAppendBufferToBuffer(originalstring->buffer, charstring->buffer);
  naDelete(charstring);
}



NA_DEF void naAppendStringFormat(NAString* originalstring, const NAUTF8Char* format, ...){
  va_list argumentlist;
  va_start(argumentlist, format);
  naAppendStringArguments(originalstring, format, argumentlist);
  va_end(argumentlist);
}


NA_DEF void naAppendStringArguments(NAString* originalstring, const NAUTF8Char* format, va_list argumentlist){
  NAString* formatstring = naNewStringWithArguments(format, argumentlist);
  naAppendBufferToBuffer(originalstring->buffer, formatstring->buffer);
  naDelete(formatstring);
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
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt8(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF int16 naParseStringInt16(const NAString* string){
  int16 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt16(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF int32 naParseStringInt32(const NAString* string){
  int32 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt32(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF int64 naParseStringInt64(const NAString* string){
  int64 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferInt64(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}


NA_DEF uint8 naParseStringUInt8(const NAString* string){
  uint8 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt8(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF uint16 naParseStringUInt16(const NAString* string){
  uint16 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt16(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF uint32 naParseStringUInt32(const NAString* string){
  uint32 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt32(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}
NA_DEF uint64 naParseStringUInt64(const NAString* string){
  uint64 retvalue;
  NABufferIterator iter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&iter, 0);
  retvalue = naParseBufferUInt64(&iter, NA_FALSE);
  naClearBufferIterator(&iter);
  return retvalue;
}

NA_DEF float naParseStringFloat(const NAString* string){
  NAUTF8Char* buf;
  NABufferIterator bufiter;
  float retvalue;
  NAInt len = naGetStringBytesize(string);
  if(len > 20){
    len = 20;
    #ifndef NDEBUG
      naError("String truncated to 20 characters");
    #endif
  }
  buf = naMalloc((len + 1) * naSizeof(NAUTF8Char));
  bufiter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&bufiter, 0);
  naReadBufferBytes(&bufiter, buf, len);
  naClearBufferIterator(&bufiter);
  retvalue = (float)atof(buf);
  naFree(buf);
  return retvalue;
}
NA_DEF double naParseStringDouble(const NAString* string){
  NAUTF8Char* buf;
  NABufferIterator bufiter;
  double retvalue;
  NAInt len = naGetStringBytesize(string);
  if(len > 20){
    len = 20;
    #ifndef NDEBUG
      naError("String truncated to 20 characters");
    #endif
  }
  buf = naMalloc((len + 1) * naSizeof(NAUTF8Char));
  bufiter = naMakeBufferAccessor(string->buffer);
  naLocateBufferFromStart(&bufiter, 0);
  naReadBufferBytes(&bufiter, buf, len);
  naClearBufferIterator(&bufiter);
  retvalue = atof(buf);
  naFree(buf);
  return retvalue;
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