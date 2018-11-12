
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.

#include "NATree.h"
#include "NAMemory.h"



NA_DEF NATree* naInitTree(NATree* tree, NATreeConfiguration* config){
  tree->config = naRetainTreeConfiguration(config);
  
  // If the config defines a callback for constructing a tree, call it.
  if(tree->config->treeConstructor){
    tree->config->treeConstructor(tree->config->data);
  }
  
  // Init the tree root.
  tree->root = NA_NULL;
  #ifndef NDEBUG
    tree->itercount = 0;
  #endif
  
  return tree;
}



NA_DEF void naEmptyTree(NATree* tree){
  #ifndef NDEBUG
    if(tree->itercount != 0)
      naError("naEmptyTree", "There are still iterators running on this tree. Did you miss a naClearTreeIterator call?");
  #endif
  if(tree->root){tree->config->nodeCoreDestructor(tree, tree->root);}
  tree->root = NA_NULL;
}



NA_DEF void naClearTree(NATree* tree){
  naEmptyTree(tree);
  // If the config sets a callback function for deleting a tree, call it.
  if(tree->config->treeDestructor){
    tree->config->treeDestructor(tree->config->data);
  }
  naReleaseTreeConfiguration(tree->config);
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
