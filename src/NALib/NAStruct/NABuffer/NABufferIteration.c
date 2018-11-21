

// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.

#include "NABuffer.h"



// /////////////////////////////////////
// Buffer Iteration
// /////////////////////////////////////



NA_HDEF const NABuffer* naGetBufferIteratorBufferConst(const NABufferIterator* iter){
  NA_UNUSED(iter);
//  return naGetPtrConst(&(iter->bufferptr));
  return NA_NULL;
}



NA_HDEF NABuffer* naGetBufferIteratorBufferMutable(NABufferIterator* iter){
  NA_UNUSED(iter);
//  return naGetPtrMutable(&(iter->bufferptr));
  return NA_NULL;
}




NA_DEF NABufferIterator naMakeBufferAccessor(const NABuffer* buffer){
  NABufferIterator iter;
//  #ifndef NDEBUG
//    NABuffer* mutablebuffer;
//    if(!buffer)
//      naCrash("naMakeBufferAccessor", "buffer is Null pointer");
//  #endif
  iter.bufferptr = naMakePtrWithDataConst(buffer);
  iter.curoffset = 0;
  iter.curbit = 0;
////  iter.linenum = 0;
//  iter.partiter = naMakeListAccessor(&(buffer->parts));
//  #ifndef NDEBUG
//    mutablebuffer = (NABuffer*)buffer;
//    mutablebuffer->itercount++;
//  #endif
//  return iter;
  return iter;
}



NA_DEF NABufferIterator naMakeBufferMutator(const NABuffer* buffer){
  NA_UNUSED(buffer);
  NABufferIterator iter;
//  #ifndef NDEBUG
//    NABuffer* mutablebuffer;
//    if(!buffer)
//      naCrash("naMakeBufferMutator", "buffer is Null pointer");
//  #endif
  iter.bufferptr = naMakePtrWithDataConst(buffer);
  iter.curoffset = 0;
  iter.curbit = 0;
////  iter.linenum = 0;
//  iter.partiter = naMakeListMutator(&(buffer->parts));
//  #ifndef NDEBUG
//    mutablebuffer = (NABuffer*)buffer;
//    mutablebuffer->itercount++;
//  #endif
  return iter;
}



NA_DEF NABufferIterator naMakeBufferModifier(NABuffer* buffer){
  NA_UNUSED(buffer);
  NABufferIterator iter;
//  #ifndef NDEBUG
//    NABuffer* mutablebuffer;
//    if(!buffer)
//      naCrash("naMakeBufferAccessor", "buffer is Null pointer");
//  #endif
  iter.bufferptr = naMakePtrWithDataMutable(buffer);
  iter.curoffset = 0;
  iter.curbit = 0;
////  iter.linenum = 0;
//  iter.partiter = naMakeListModifier(&(buffer->parts));
//  #ifndef NDEBUG
//    mutablebuffer = (NABuffer*)buffer;
//    mutablebuffer->itercount++;
//  #endif
  return iter;
}



NA_DEF void naClearBufferIterator(NABufferIterator* iter){
  NA_UNUSED(iter);
//  naClearListIterator(&(iter->partiter));
//  #ifndef NDEBUG
//    NABuffer* mutablebuffer;
//    mutablebuffer = naGetBufferIteratorBufferMutable(iter);
//    if(mutablebuffer->itercount == 0)
//      naError("naClearBufferIterator", "Too many clears: Buffer has no iterators to clear.");
//    mutablebuffer->itercount--;
//  #endif
}



NA_DEF NAInt naTellBuffer(const NABufferIterator* iter){
  NA_UNUSED(iter);
//  #ifndef NDEBUG
//    if(iter->curbit != 0)
//      naError("naTellBuffer", "Buffer bitcount is not null.");
//  #endif
//  return iter->curoffset;
  return 0;
}



NA_DEF void naSeekBufferAbsolute(NABufferIterator* iter, NAInt offset){
  NA_UNUSED(iter);
  NA_UNUSED(offset);
//  iter->curoffset = offset;
//  if(!naIsBufferEmpty(naGetBufferIteratorBufferConst(iter))){
//    naLocateBufferPartOffset(&(iter->partiter), offset);
//  }
  return;
}



NA_DEF void naSeekBufferRelative(NABufferIterator* iter, NAInt offset){
  NA_UNUSED(iter);
  NA_UNUSED(offset);
//  NAInt absoffset = iter->curoffset + offset;
//  naSeekBufferAbsolute(iter, absoffset);
  return;
}



NA_DEF void naSeekBufferSource(NABufferIterator* iter, NAInt offset){
  NA_UNUSED(iter);
  NA_UNUSED(offset);
//  NAInt absoffset = naGetBufferIteratorBufferConst(iter)->srcoffset + offset;
//  naSeekBufferAbsolute(iter, absoffset);
  return;
}



NA_DEF void naSeekBufferFromStart(NABufferIterator* iter, NAInt offset){
  NA_UNUSED(iter);
  NA_UNUSED(offset);
//  NAInt absoffset = naGetBufferIteratorBufferConst(iter)->range.origin + offset;
//  naSeekBufferAbsolute(iter, absoffset);
  return;
}



NA_DEF void naSeekBufferFromEnd(NABufferIterator* iter, NAInt offset){
  NA_UNUSED(iter);
  NA_UNUSED(offset);
//  NAInt absoffset = naGetRangeiEnd(naGetBufferIteratorBufferConst(iter)->range) + offset;
//  naSeekBufferAbsolute(iter, absoffset);
  return;
}



NA_DEF NABool naIterateBuffer(NABufferIterator* iter, NAInt step){
  NA_UNUSED(iter);
  NA_UNUSED(step);
//  const NABufferPart* part;
//  const NABuffer* buffer = naGetBufferIteratorBufferConst(iter);
//  #ifndef NDEBUG
//    if(!step)
//      naError("naIterateBuffer", "step is zero");
//  #endif
//  if(naIsBufferEmpty(buffer)){
//    return NA_FALSE;
//  }
//  part = naGetListCurConst(&(iter->partiter));
//  if(!part){
//    if(step > 0){
//      naLocateListFirst(&(iter->partiter));
//      part = naGetListCurConst(&(iter->partiter));
//      iter->curoffset = buffer->range.origin;
//    }else{
//      naLocateListLast(&(iter->partiter));
//      part = naGetListCurConst(&(iter->partiter));
//      iter->curoffset = naGetRangeiEnd(buffer->range);
//    }
//  }
//  iter->curoffset += step;
//  if(step > 0){
//    while(part && !naContainsBufferPartOffset(part, iter->curoffset)){
//      naIterateList(&(iter->partiter));
//      part = naGetListCurConst(&(iter->partiter));
//    }
//  }else{
//    while(part && !naContainsBufferPartOffset(part, iter->curoffset)){
//      naIterateListBack(&(iter->partiter));
//      part = naGetListCurConst(&(iter->partiter));
//    }
//  }
//  return (part != NA_NULL);
  return NA_FALSE;
}



NA_DEF NABool naIsBufferAtEnd(const NABufferIterator* iter){
  NA_UNUSED(iter);
//  const NABuffer* buffer = naGetBufferIteratorBufferConst(iter);
//  #ifndef NDEBUG
//    if(iter->curoffset > naGetRangeiEnd(buffer->range))
//      naError("naIsBufferAtEnd", "Iterator is far off the end");
//  #endif
//  return (iter->curoffset == naGetRangeiEnd(buffer->range));
  return NA_FALSE;
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
