/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"



/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode) {
   Node_T oNParent;
   Path_T oPNPath; /* node path */
   Path_T oPPPath; /* parent path */
   size_t ulIndex;
   size_t childArrSize; 

   /* Sample check: a NULL pointer is not a valid node */
   if(oNNode == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if(oNParent != NULL) {
      oPNPath = Node_getPath(oNNode);
      oPPPath = Node_getPath(oNParent);

      if(Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
         Path_getDepth(oPNPath) - 1) {
         fprintf(stderr, "P-C nodes don't have P-C paths: (%s) (%s)\n",
                 Path_getPathname(oPPPath), Path_getPathname(oPNPath));
         return FALSE;
      }
   }

   /* COUNT CHECK: Node_getNumChildren returns a number equal to the*/

   /* POINTER CHECK: oNNode's children's all recognize oNNode as it's parent.*/
   childArrSize = Node_getNumChildren(oNNode);
   for (ulIndex = 0; ulIndex < childArrSize; ulIndex++) {
      Node_T oNInspect;
      Node_getChild(oNNode, ulIndex, &oNInspect);
      if (oNInspect != NULL) {
         if (Node_compare(oNNode, Node_getParent(oNInspect)) != 0) {
         fprintf(stderr, "Conflicting parent/child relationship. ");
         return FALSE;
         }
      }
   }

   /* PATH CHECK: Children are in lexicographical order. */
   if (childArrSize > 1){
      for (ulIndex = 0; ulIndex < childArrSize - 1; ulIndex++) {
         Node_T oNInspect1;
         Node_T oNInspect2;
         Path_T oPInspectPath1;
         Path_T oPInspectPath2;

         Node_getChild(oNNode, ulIndex, &oNInspect1);
         Node_getChild(oNNode, ulIndex + 1, &oNInspect2);
         oPInspectPath1 = Node_getPath(oNInspect1);
         oPInspectPath2 = Node_getPath(oNInspect2);
         if (Path_comparePath(oPInspectPath1, oPInspectPath2) > 0) {
            fprintf(stderr, "Lexicographical children order violated. ");
            return FALSE;
         }
      }
   }

   /* PATH CHECK: oNNode's children don't have duplicate path names. 
      Assume lexicographical order check has passed. */
   if (childArrSize > 1) {
      for (ulIndex = 0; ulIndex < childArrSize - 1; ulIndex++) {
         Node_T oNInspect1;
         Node_T oNInspect2;
         Path_T oPInspectPath1;
         Path_T oPInspectPath2;

         Node_getChild(oNNode, ulIndex, &oNInspect1);
         Node_getChild(oNNode, ulIndex + 1, &oNInspect2);
         oPInspectPath1 = Node_getPath(oNInspect1);
         oPInspectPath2 = Node_getPath(oNInspect2);
         if (Path_comparePath(oPInspectPath1, oPInspectPath2) == 0) {
            fprintf(stderr, "Duplicate paths. ");
            return FALSE;
         }
      }
   }

   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode
   and counts the number of nodes in the tree, storing that number
   in the pointer *pulCounter.

   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean CheckerDT_treeCheck(Node_T oNNode, size_t *pulCounter) {
   size_t ulIndex;
   /* check for double counting but increment counter as needed.
   ensure increment is only once per treecheck call. */

   if(oNNode!= NULL) {
      /* Increment counter of nodes seen. */
      (*pulCounter)++;
      /* fprintf(stderr, "counter: %d", *pulCounter); */
      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* Recur on every child of oNNode */
      for(ulIndex = 0; ulIndex < Node_getNumChildren(oNNode); ulIndex++)
      {
         Node_T oNChild = NULL;
         int iStatus = Node_getChild(oNNode, ulIndex, &oNChild);

         if(iStatus != SUCCESS) {
            fprintf(stderr, "getNumChildren claims more children than getChild returns\n");
            return FALSE;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */

            /*can pass in count. deref in treecheck, update, put back in*/
         if(!CheckerDT_treeCheck(oNChild, pulCounter))
            return FALSE;
      }
   }
   return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount) {
   boolean result;
   size_t *pulCounter = malloc(sizeof(size_t));
   (*pulCounter) = 0;
   /* fprintf(stderr, "reported count: %d", ulCount); */

   /* check that ulCount matches the pointer to int*/
   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!bIsInitialized) {
      /*If tree is uninitialized, count should be 0. */
      if(ulCount != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }
      /* If tree is uninitialized, root should be null. */
      else if (oNRoot != NULL) {
         fprintf(stderr, "Not initialized, but root is not NULL\n");
         return FALSE;
      }
   }
   /* Initialized cases */
   else if (bIsInitialized) {
      /* If the root is null, ulCount should be 0. */ 
      if (oNRoot == NULL && ulCount != 0) {
         fprintf(stderr, "Root is NULL, but count is not 0\n");
         return FALSE;
      }
      /* If ulCount is 0, the root should be null. */ 
      else if (ulCount == 0 && oNRoot != NULL) {
         fprintf(stderr, "Count is 0, but root is not NULL\n");
         return FALSE;
      }
      /* The root's parent should be null if root isn't NULL. */
      else if (oNRoot != NULL && Node_getParent(oNRoot) != NULL) {
         fprintf(stderr, "Root's parent is not null. \n");
         return FALSE;
      }
   }

   /* Now checks invariants recursively at each node from the root. */
   /* Store result in variable to return later. */
   result = CheckerDT_treeCheck(oNRoot, pulCounter);

   /* Check that the count in tree is correct. */
   if (*pulCounter != ulCount) {
         fprintf(stderr, "Wrong number of nodes in tree. \n");
         return FALSE;
      }
   return result;
}
