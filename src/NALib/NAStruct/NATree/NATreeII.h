
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.

// Returns true if the iterator is located at a position which overlaps the
// range/rect/box given in limit.
typedef NABool (*NATreeNodeLimitTester)(NATreeIterator* iter, const void* limit);

#define NA_TREE_NODE_CHILD_NULL 0x00
#define NA_TREE_NODE_CHILD_NODE 0x02
#define NA_TREE_NODE_CHILD_LEAF 0x03

#define NA_TREE_NODE_CHILD_AVAILABLE_MASK 0x02
#define NA_TREE_NODE_CHILD_MASK 0x03

struct NATreeConfiguration{
  NARefCount                  refcount;
  NAMutator                   destructor;
  NATreeContructorCallback    treeconstructor;
  NATreeDestructorCallback    treedestructor;
  NATreeLeafConstructor       leafconstructor;
  NATreeLeafDestructor        leafdestructor;
//  NAQuadTreeDataCopier      datacopier;
  NATreeNodeConstructor       nodeconstructor;
  NATreeNodeDestructor        nodedestructor;
//  NAQuadTreeChildChanged    childchanged;
  NAPtr                       userdata;
//  int16                     baseleafexponent;
  NAInt                       flags;
  
  NAInt                   childpernode;
  NATreeNodeLimitTester   limittester;
  #ifndef NDEBUG
    NAInt                 debugflags;
  #endif
};

typedef struct NATreeNode NATreeNode;
struct NATreeNode{
  double key;
  NATreeNode* parent;
  NAInt indxinparent;
  NATreeNode* childs[2];
  NAPtr userdata;
  NAInt flags;
  #ifndef NDEBUG
    NAInt itercount;
  #endif
};
NA_EXTERN_RUNTIME_TYPE(NATreeNode);

struct NATreeIterator{
  NAPtr tree;
  NATreeNode* node;
  NAInt childindx;
  NAInt flags;
};

struct NATree{
  NATreeConfiguration* config;
  NATreeNode* root;
  #ifndef NDEBUG
    NAInt itercount;
  #endif
};



// NATreeNode
NA_HAPI NATreeNode* naAllocTreeNode(NATree* tree, NATreeNode* parent, NAInt indxinparent, double key, void* leaf);
NA_HAPI void naDeallocTreeNode(NATree* tree, NATreeNode* node);
NA_HIAPI NABool naIsNodeChildLeaf(NATreeNode* node, NAInt childindx);
NA_HIAPI NABool naHasNodeChild(NATreeNode* node, NAInt childindx);
NA_HAPI NABool naLocateTreeNode(NATreeIterator* iter, double key);

// NATreeConfiguration
NA_HAPI NATreeConfiguration* naRetainTreeConfiguration(NATreeConfiguration* config);



NA_HIDEF NABool naIsNodeChildLeaf(NATreeNode* node, NAInt childindx){
  return naTestFlagi(node->flags, NA_TREE_NODE_CHILD_LEAF << (childindx * 2));
}
NA_HIDEF NABool naHasNodeChild(NATreeNode* node, NAInt childindx){
  return naTestFlagi(node->flags, NA_TREE_NODE_CHILD_AVAILABLE_MASK << (childindx * 2));
}
NA_HIDEF void naSetNodeChildType(NATreeNode* node, NAInt childindx, NAInt childtype){
  naSetFlagi(&(node->flags), NA_TREE_NODE_CHILD_MASK << (childindx * 2), NA_FALSE);
  naSetFlagi(&(node->flags), childtype << (childindx * 2), NA_TRUE);
}



NABool naTestTreeLimitBinary(NATreeIterator* iter, const void* limit);

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
