/*--------------------------------------------------------------------*/
/* nodeDT.h                                                           */
/* Author: Connor Brown and Laura Hwa                                 */
/*--------------------------------------------------------------------*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <stddef.h>
#include "a4def.h"
#include "path.h"

/* A Node_T is a node in a File Tree */
typedef struct node *Node_T;

/*
  Creates a new node with path oPPath and parent oNParent. If isFile is 
  TRUE, new node is created as a file with content pvContents and 
  content size ulContentSize. If isFile is FALSE, contents is 
  initialized to NULL and content size 0. Returns an int SUCCESS status 
  and sets *poNResult to be the new node if successful. Otherwise, sets 
  *poNResult to NULL and returns status:
  * MEMORY_ERROR if memory could not be allocated to complete request
  * CONFLICTING_PATH if oNParent's path is not an ancestor of oPPath
  * NO_SUCH_PATH if oPPath is of depth 0
                 or oNParent's path is not oPPath's direct parent
                 or oNParent is NULL but oPPath is not of depth 1
  * ALREADY_IN_TREE if oNParent already has a child with this path
  * NOT_A_DIRECTORY if oNParent is a file
*/
int Node_new(Path_T oPPath, Node_T oNParent, boolean isFile, 
            void *pvContents, size_t ulContentSize, Node_T *poNResult);

/*
  Destroys and frees all memory allocated for the subtree rooted at
  oNNode, i.e., deletes this node and all its descendents. Returns the
  number of nodes deleted.
*/
size_t Node_free(Node_T oNNode);

/* Returns the path object representing oNNode's absolute path. */
Path_T Node_getPath(Node_T oNNode);

/*
  Returns TRUE if oNParent has a child with path oPPath. Returns
  FALSE if it does not.

  If oNParent has such a child, stores in *pulChildID the child's
  identifier (as used in Node_getChild). If oNParent does not have
  such a child, stores in *pulChildID the identifier that such a
  child _would_ have if inserted.
*/
boolean Node_hasChild(Node_T oNParent, Path_T oPPath,
                         size_t *pulChildID);

/* Returns TRUE if oNNode is a file, FALSE if directory. */
boolean Node_isFile(Node_T oNNode);

/* Returns the contents of oNNode. If oNNode is a directory, returns 
   NULL. */
void *Node_getContent(Node_T oNNode);

/* Returns the content size of oNNode. If oNNode is a directory, returns 
   0. */
size_t Node_getContentSize(Node_T oNNode);

/* Replaces the content of oNNode with pvNewContent, only if oNNode is
   a file. Returns the previous content, if oNNode is a file, or NULL,
   if oNNode is a directory. */
void *Node_setContent(Node_T oNNode, void *pvNewContent);

/* Replaces the content size of oNNode with ulNewContentSize, only if 
   oNNode is a file. Returns the previous content size, if oNNode is a 
   file, or 0, if oNNode is a directory. */
size_t Node_setContentSize(Node_T oNNode, size_t ulNewContentSize);

/* Returns the number of children that oNParent has. */
size_t Node_getNumChildren(Node_T oNParent);

/*
  Returns an int SUCCESS status and sets *poNResult to be the child
  node of oNParent with identifier ulChildID, if one exists.
  Otherwise, sets *poNResult to NULL and returns status:
  * NO_SUCH_PATH if ulChildID is not a valid child for oNParent
  * NOT_A_DIRECTORY if oNParent is a file
*/
int Node_getChild(Node_T oNParent, size_t ulChildID,
                  Node_T *poNResult);

/*
  Returns a the parent node of oNNode.
  Returns NULL if oNNode is the root and thus has no parent.
*/
Node_T Node_getParent(Node_T oNNode);

/*
  Returns a string representation for oNNode, or NULL if
  there is an allocation error.

  Allocates memory for the returned string, which is then owned by
  the caller!
*/
char *Node_toString(Node_T oNNode);

#endif