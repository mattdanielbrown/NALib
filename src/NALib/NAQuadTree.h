
// This file is part of NALib, a collection of C and C++ source code
// intended for didactical purposes. Full license notice at the bottom.

#ifndef NA_QUADTREE_INCLUDED
#define NA_QUADTREE_INCLUDED
#ifdef __cplusplus 
  extern "C"{
#endif

#include "NACoord.h"
#include "NAMemory.h"

// An NAQuadTree is a container struct capable of storing a two-dimensional
// object of any bytesize. The content of the object is divided into equally
// sized quadratic chunks with a given minimal length in one dimension. These
// chunks are the leafs of the tree. Aside from leafes, an NAQuadTree stores
// parent nodes. Every leaf has a parent node and every node may have another
// parent node.
//
// The child nodes are sometimes referred to as segments which are stored
// internally in the following order:
// segment 0: (lower x, lower y)
// segment 1: (upper x, lower y)
// segment 2: (lower x, upper y)
// segment 3: (upper x, upper y)
//
// There are two ways of accessing and manipulating the tree: Direct access of
// the leafes upon coordinate location or iterating over all leaves. The tree
// uses some internal variables to make the search and iteration fast.
//
// Manipulating an NAQuadTree is done in the leafes. When a leaf has been
// altered, a message is sent to the parent nodes. This is called bubbling.
// You can intercept these bubbling messages implementing the appropriate
// callback functions.
//
// Regarding Const and Mutable: This datastructure can not be manipulated like
// other datastructures from NALib. You do not add single elements but instead,
// the tree asks you to provide chunks when necessary. Therefore, the data
// stored is always mutable. You can nontheless access the tree const or
// mutable. This is important when for example, if you just have a const tree
// but require a chunk of it for reading.



// ///////////////////////////////////////////
// CALLBACKS
// ///////////////////////////////////////////

// When creating an NAQuadTree, you can create various callbacks which are
// stored in a NAQuadTreeCallbacks structure. Here are the signatures for 
// these callbacks:

// Allocation function which allocate your leaf-chunks. This callback always
// must be present and shall always return something. Returning Null is a bad
// thing. This callback will be called when creating new chunks (whenever a
// NAQuadTree gets expanded) or when duplicating an NAQuadTree.
// The parameters given are the origin of the data as well as the (always
// square) leaflength of the data rectangle. When copydata is non-Null, it
// points to already existing chunk data which shall be copied to the new
// chunk. This will be the case when duplicating an NAQuadTree.
// The userdata parameter contains whatever you defined upon creation of the
// tree within the NAQuadTreeCallbacks struct. With this, it is for example
// possible to allocate data from a user defined pool-structure.
// Note that both origin and leaflength are just here for information. You may
// or may not use them. And you will probably not need to store them. But you
// may if you really want to.
// The leaflength given will always be the leaflength declared at creation of
// an NAQuadTree.
// Also note that the coordinates of origin are guaranteed to be a multiple of
// leaflength.
typedef void* (*NAQuadTreeLeafAllocator)(   NAPosi origin,
                                             NAInt leaflength,
                                             void* userdata,
                                       const void* copydata);

// The destruction function of your leaf-chunks. This callback always must be
// present. The pointer given is a pointer created when NAQuadTreeLeafAllocator
// was called. The userdata is the same as desribed in the allocator function.
typedef void  (*NAQuadTreeLeafDeallocator)(  void* leafdata,
                                             void* userdata);

// This callback is required when shifting an NAQuadTree. You are given two
// chunk data pointers, two origins and a size.
// Your task is to copy the data in srcdata starting at srcorigin to dstdata
// starting at dstorigin. The size denotes the size of the rectangle which
// shall be copied. The origins are given relative to the chunk origin, meaning
// no matter where the actual origin of the chunk is, the given origins will
// always be greaterequal zero. Additionally, all coordinates of the two
// rectangles are guaranteed to be inside the corresopnding chunk. Note that
// the size does not necessarily denotes a square. It can denote any rectangle.
// The leaflength is given to you as an additional information. It corresponds
// to the leaflength used when the NAQuadTree was created.
// Example (numbers are approximate):
//          dstdata           srcdata
//        O---------+       O---------+       
//        |         |       |s----+   |       srcorigin s: (1,1)
//        |    d----+       ||    |   |       dstorigin d: (3,2)
//        |    |    | <---- |+----+   |       size: (4,2)
//        |    +----+       |         |       leaflength: 8
//        +---------+       +---------+       

typedef void  (*NAQuadTreeDataCopier)( const void* dstdata,
                                            NAPosi dstorigin,
                                       const void* srcdata,
                                            NAPosi srcorigin,
                                           NASizei size,
                                             NAInt leaflength);

// This callback is called when a quad tree creates an internal tree node other
// than a leaf.
// Internal nodes can NOT be manipulated directly but you are allowed to store
// any data with every node if desired. You can return a pointer to any data,
// even a Null pointer. This data pointer will be available to
// NAQuadTreeLeafChanged, NAQuadTreeChildChanged and NAQuadTreeNodeDeallocator.
// An internal node internally stores  pointers to childnodes or leafes. The
// origin parameter denotes the origin of the (lowerx, lowery)-node and the
// childnodesize denotes the (always square) rectangle the childnodes have.
// Note that both origin and nodesize are just here for information. You may or
// may not use them. And you will probably not need to store them. But you may
// if you really want to.
typedef void* (*NAQuadTreeNodeAllocator)( NAPosi origin,
                                           NAInt childnodesize);

// The node destructor is called before a quad tree ultimately deletes an
// internal node node.
// The pointer created with NAQuadTreeNodeAllocator will be sent to this
// function such that you can deallocate the memory if necessary. Note that
// this function is NOT called if a node stores a NULL pointer.
typedef void  (*NAQuadTreeNodeDeallocator)(void* nodedata);

// Whenever you altered a leaf and want to propagate the change over the whole
// tree, you call naUpdateQuadTreeCurrent which in turn will call the following
// two callback functions. Both will be called with the parental (internal) node
// data you may have stored with NAQuadTreeNodeAllocator. Additionally, you are
// given all four child data pointers as an array as well as a segment index
// denoting which of the four childs has caused the calling. See segment index
// description above.
//
// The two functions differ on whether the childs are leafs or nodes. The data
// given are either pointers to leaf chunk data or pointers to node data you
// stored with NAQuadTreeNodeAllocator. All individual pointers can be Null
// meaning there is no leaf stored or Null has explicitely been stored as a
// node. This means, these functions will be called EVEN IF Null was stored.
//
// Segment is a number in [0 - 3] denoting the child which caused the call.
// The value -1 is given if there was no particular child causing the call.
// This happens for example, when a tree gets copied as a whole.
//
// This function can return NA_TRUE if the message shall be propagated towards
// the next parent (bubbling). If NA_FALSE is returned, the tree update stops
// immediately. This return value though is ignored when copying a whole tree,
// where these callbacks are called whenever a node has been fully constructed.
typedef NABool(*NAQuadTreeLeafChanged)(      void* nodedata,
                                             NAInt segment,
                                 const void* const leafdata[4]);
typedef NABool(*NAQuadTreeChildChanged)(     void* nodedata,
                                             NAInt segment,
                                 const void* const childdata[4]);

// This callback is called when the data of a leaf chunk shall be serialized.
// You must always return the number of bytes required to store the chunk data.
// You may use the leaflength given for the calculation of the number of bytes.
// The leaflength always is the leaflength declared upon creation of the
// NAQuadTree.
//
// When buffer is a Null pointer, just return the required number of bytes.
// When buffer is NOT a Null-Pointer, this buffer is guaranteed to have the
// required number of bytes and you can copy data to this buffer.
//
// Therefore, when you call naSerializeQuadTree, this callback will be called
// for every chunk twice: First to akquire the required number of bytes and
// second to actually store the data.
typedef uint64(*NAQuadTreeSerializer)        (void* buffer,
                                        const void* leafdata,
                                              NAInt leaflength);

// This callback is called when a buffer shall be deserialized which
// previously had been serialized with NAQuadTreeSerializer. Create a leaf
// chunk and return a pointer to the chunk data. The origin and (always
// quadratic) leaflength are given to you as an additional information.
//
// Note that this callback is very similar to NAQuadTreeLeafAllocator but
// here, buffer denotes not a chunk which shall be copied but rather more a
// buffer which need to be deserialized. These can be two different things.
typedef void* (*NAQuadTreeDeserializer)(     NAPosi origin,
                                              NAInt leaflength,
                                        const void* buffer);




// This is the callback struct you can use to create an NAQuadTree. After
// creation, these functions can no longer be changed.
typedef struct NAQuadTreeCallbacks_struct{
  NAQuadTreeLeafAllocator   leafallocator;    // must be present.
  NAQuadTreeLeafDeallocator leafdeallocator;  // must be present.
  NAQuadTreeDataCopier      datacopier;
  NAQuadTreeNodeAllocator   nodeallocator;
  NAQuadTreeNodeDeallocator nodedeallocator;
  NAQuadTreeLeafChanged     leafchanged;
  NAQuadTreeChildChanged    childchanged;
  NAQuadTreeSerializer      serialize;
  NAQuadTreeDeserializer    deserialize;
  void*                     userdata;
} NAQuadTreeCallbacks;



// Typedefs of opaque types which should not be visible to the programmer.
typedef struct NAQuadTree NAQuadTree;
typedef struct NAQuadTreeIterator NAQuadTreeIterator;




// //////////////////////////////
// FUNCTIONS
// //////////////////////////////

// Initializes an empty NAQuadTree with the given leaflength and callbacks.
NA_API NAQuadTree* naInitQuadTree(              NAQuadTree* tree,
                                                      NAInt leaflength,
                                        NAQuadTreeCallbacks callbacks);

// Initializes newtree by duplicating the given copytree.
NA_API NAQuadTree* naInitQuadTreeCopy(          NAQuadTree* newtree,
                                          const NAQuadTree* copytree);

// Duplicates the given copytree, but only containing the chunks which are
// present in masktree.
NA_DEF NAQuadTree* naInitQuadTreeCopyMasked(    NAQuadTree* newtree,
                                          const NAQuadTree* copytree,
                                          const NAQuadTree* masktree);

// Duplicates the given copytree but shifts all contents by the given shift.
NA_API NAQuadTree* naInitQuadTreeCopyShifted(   NAQuadTree* newtree,
                                          const NAQuadTree* copytree,
                                                    NASizei shift);

// Serializes a full NAQuadTree.
// When buf is Null, the total amount of bytes required for the whole tree
// including all structural data is returned in bytesize.
// When buf is not Null, the tree serializes into the given buffer. You must
// send the bytesize you received from the first call again!
NA_API void naSerializeQuadTree(            const NAQuadTree* tree,
                                                        void* buf,
                                                      uint64* bytesize);
// Deserializes a full NAQuadTree
NA_API NAQuadTree* naInitQuadTreeWithDeserialization(NAQuadTree* tree,
                                                      const void* buf,
                                        NAQuadTreeCallbacks callbacks);

// Empties the tree
NA_IAPI void naEmptyQuadTree(                   NAQuadTree* tree);

// Clears the tree
NA_API void naClearQuadTree(                    NAQuadTree* tree);

// Returns true if the tree is empty
NA_IAPI NABool naIsQuadTreeEmpty(         const NAQuadTree* tree);

// Returns the leaf length in one dimension
NA_IAPI NAInt naGetQuadTreeLeafLength(    const NAQuadTree* tree);

// Returns the callbacks (by copying the function pointers)
NA_IAPI NAQuadTreeCallbacks naGetQuadTreeCallbacks(const NAQuadTree* tree);



// //////////////////////////
// Accessors and Mutators:

// Returns the root node data, if available. If no root or no root data are
// available, NA_NULL is returned.
NA_IAPI void* naGetQuadTreeRootNodeData(NAQuadTree* tree);

// Returns the chunk containing the given coord. If no chunk with the desired
// coord exists, NA_NULL is returned. When create is true, the chunk will be
// created and returned. If the origin pointer is not Null it will be filled
// with the origin information of the chunk returned. When didcreate is a
// non-Null-pointer and create is true, didcreate returns whether the leaf has
// indeed been created or not.
NA_API const void* naGetQuadTreeLeafConst(  const NAQuadTree* tree,
                                                      NAPosi  coord,
                                                      NAPosi* origin);
NA_API       void* naGetQuadTreeLeafMutable(      NAQuadTree* tree,
                                                      NAPosi  coord,
                                                      NABool  create,
                                                     NABool*  didcreate,
                                                      NAPosi* origin);

// Removes the leaf containing the specified coordinate.
NA_API void naRemoveQuadTreeLeaf(NAQuadTree* tree, NAPosi coord);

// Starts at the leaf containing the given coord and traverses upwards
// (bubbling) the tree by calling the appropriate callback functions of all
// nodes. When the callback functions return NA_FALSE, bubbling stops.
NA_API void naUpdateQuadTreeAtCoord(  NAQuadTree* tree, NAPosi coord);

// Same thing but updates every leaf in the whole tree. All messages will
// bubble as long as there is at least one child per node requesting a bubble.
NA_API void naUpdateQuadTree(         NAQuadTree* tree);



// ////////////////////////
// Calling one and the same function on a large portion of the tree.

// Function pointer used for the set iteration naSetQuadTreeInRect. This
// callback is called for multiple chunks of a tree.
// When called, dstdata is the chunk data, rect is the rect within the data
// shall be set and leaflength is the leaf length in one dimension given upon
// creation of the tree.
// The userdata corresponds to whatever has been given in naSetQuadTreeInRect.
// The origin of the rect is given relative to the origin of the chunk.
// Therefore, all positions are greaterequal zero. All coordinates of rect are
// guaranteed to be inside the leaf.
typedef void  (*NAQuadTreeDataSetter)( const void* dstdata,
                                           NARecti rect,
                                             NAInt leaflength,
                                       const void* userdata);

// Calls the given NAQuadTreeDataSetter function for every chunk in the tree
// which is partially or fully overlapped with the given rect. Will create
// the chunks if necessary.
NA_API void naSetQuadTreeInRect(          NAQuadTree* tree,
                                              NARecti rect,
                                 NAQuadTreeDataSetter datasetter,
                                          const void* userdata);



// ////////////////////////////////////
// Iterators
//
// You can iterate over every leaf of a whole tree like this:
//
// NAQuadTreeIterator iter = naMakeQuadTreeIteratorMutable(tree);
// while(naIterateQuadTree(&iter)){
//   MyLeafData* data = naGetQuadTreeCurrentMutable(&iter);
//   NAPosi origin = naGetQuadTreeCurrentOrigin(&iter);
//   // do stuff with the leaf data.
// }
// naClearQuadTreeIterator(&iter);
//
// You can choose to have an Accessor, a Mutator or a Modifier as Iterator.
// Only with the Modifier can leafes be removed with the naRemoveQuadTreeLeaf
// function.
//
// Beware to always use naClearQuadTreeIterator. Otherwise NALib will emit
// warnings that there are still iterators running on the struct if NDEBUG is
// undefined.

NA_IAPI NAQuadTreeIterator naMakeQuadTreeAccessor(const NAQuadTree* tree);
NA_IAPI NAQuadTreeIterator naMakeQuadTreeMutator (      NAQuadTree* tree);
NA_IAPI void naClearQuadTreeIterator(NAQuadTreeIterator* iter);

// Iterates to the next leaf and returns NA_TRUE if there is one, NA_FALSE if
// the iteration is over. The leafes will be visited like they are stored
// within the tree. If you need axis ordered traversal, maybe have a look at
// the naIterateQuadTreeSteps function.
//
// The limit denotes the rectangle the iteration takes place in. Only leafes
// which partially or completely overlap with the limit rect will be visited.
// If limit is NA_NULL, all leafes will be visited.
NA_API NABool naIterateQuadTree(NAQuadTreeIterator* iter, const NARecti* limit);

// Moves the iterator to the leaf containing the given coord. If coord is not
// found in the tree, NA_FALSE ist returned and a leaf is currently selected
// which is somewhat close to the coordinates.
NA_API NABool naLocateQuadTreeCoord(NAQuadTreeIterator* iter, NAPosi coord);

// Moves the iterator relative to the current position. Each step can be
// positive or negative. If the iterator was not in any specific position
// before, it will be rather arbitrarily after. Use naLocateQuadTreeCoord
// before using this function.
NA_API NABool naIterateQuadTreeSteps( NAQuadTreeIterator* iter,
                                                    NAInt stepx,
                                                    NAInt stepy);

// You can retrieve the current leaf or the origin of the current leaf with
// these functions:
NA_IAPI const void* naGetQuadTreeCurrentConst(NAQuadTreeIterator* iter);
NA_IAPI void* naGetQuadTreeCurrentMutable(NAQuadTreeIterator* iter);
NA_IAPI NAPosi naGetQuadTreeCurrentOrigin(NAQuadTreeIterator* iter);







// Inline implementations are in a separate file:
#include "NAStruct/NAQuadTreeII.h"





#ifdef __cplusplus 
  } // extern "C"
#endif
#endif // NA_QUADTREE_INCLUDED

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
