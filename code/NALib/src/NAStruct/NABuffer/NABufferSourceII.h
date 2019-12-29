
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.


// This file contains inline implementations of the file NABuffer.h
// Do not include this file directly! It will automatically be included when
// including "NABufferII.h"



// Flags for the buffer source:
#define NA_BUFFER_SOURCE_RANGE_LIMITED        0x01
#define NA_BUFFER_SOURCE_DEBUG_FLAG_IMMUTABLE 0x80



struct NABufferSource{
  NABufferFiller    buffiller;      // Fill function filling memory.
  NABuffer*         buffer;         // The underlying buffer, if any.
  void*             data;           // data sent to filler and destructor.
  NAMutator         datadestructor; // Data destructor.
  NAUInt            flags;          // Flags for the source
  NARangei          limit;          // Source limit (used if flag set)
};
NA_EXTERN_RUNTIME_TYPE(NABufferSource);



NA_DEF void naSetBufferSourceData(NABufferSource* source, void* data, NAMutator datadestructor){
  #ifndef NDEBUG
    if(source->flags & NA_BUFFER_SOURCE_DEBUG_FLAG_IMMUTABLE)
      naError("Source already used in a buffer. Mayor problems may occur in the future");
  #endif
  source->data = data;
  source->datadestructor = datadestructor;
}



NA_DEF void naSetBufferSourceLimit(NABufferSource* source, NARangei limit){
  #ifndef NDEBUG
    if(source->flags & NA_BUFFER_SOURCE_RANGE_LIMITED)
      naError("Source already has a limit");
    if(source->flags & NA_BUFFER_SOURCE_DEBUG_FLAG_IMMUTABLE)
      naError("Source already used in a buffer. Mayor problems may occur in the future");
  #endif
  source->flags |= NA_BUFFER_SOURCE_RANGE_LIMITED;
  source->limit = limit;
}



NA_HDEF NABuffer* naGetBufferSourceUnderlyingBuffer(NABufferSource* source){
  return source->buffer;
}



// Returns NA_TRUE if the range is a valid limiting range.
NA_HDEF NABool naIsBufferSourceLimited(const NABufferSource* source){
  return (source->flags & NA_BUFFER_SOURCE_RANGE_LIMITED);
}



NA_HDEF NARangei naGetBufferSourceLimit(const NABufferSource* source){
  #ifndef NDEBUG
    if(!naIsBufferSourceLimited(source))
      naError("source is not limited");
  #endif
  return source->limit;
}



NA_HDEF void naFillSourceBuffer(const NABufferSource* source, void* dst, NARangei range){
  if(source && source->buffiller){
    source->buffiller(dst, range, source->data);
  }
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