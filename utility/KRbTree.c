
#include <assert.h>
#include "KRbTree.h"
#include "KList.h"

KRbNode __RBTreeNilNode = {
    .__parent_color = RBNODE_COLOR_BLACK,
    .left = &__RBTreeNilNode,
    .right = &__RBTreeNilNode
};

KRbNode *RBTreeNil = &__RBTreeNilNode;

KRbNode *KRbMinimum(KRbNode *node) {
    assert(node);
    while (KRbNotNil(node->left)) {
        node = node->left;
    }
    return node;
}

KRbNode *KRbMaximum(KRbNode *node) {
    assert(node);
    while (KRbNotNil(node->right)) {
        node = node->right;
    }
    return node;
}

KRbNode *KRbSuccessor(KRbNode *node) {
    if (KRbNotNil(node->right)) {
        return KRbMinimum(node->right);
    }

    KRbNode *reval = KRbParent(node);
    while(KRbNotNil(reval) && node == reval->right) {
        node = reval;
        reval = KRbParent(node);
    }
    return reval;
}

KRbNode *KRbPredecessor(KRbNode *node) {
    if (KRbNotNil(node->left)) {
        return KRbMaximum(node->left);
    }

    KRbNode *reval = KRbParent(node);
    while(KRbNotNil(reval) && node == reval->left) {
        node = reval;
        reval = KRbParent(node);
    }
    return reval;
}

static inline void KRbLeftRotate(KRbRoot *root,KRbNode *node) {
    if (root == NULL || node == NULL) {
        return ;
    }

    KRbNode *rightChild = node->right;
    node->right = rightChild->left;

    if(rightChild->left != RBTreeNil){
        KRbSetParent(rightChild->left,node);
    }

    KRbNode *parent = KRbParent(node);
    KRbSetParent(rightChild,parent);
    if (parent == RBTreeNil) {
        root->node = rightChild;
    } else if(node == parent->left) {
        parent->left = rightChild;
    } else {
        parent->right = rightChild;
    }

    rightChild->left = node;
    KRbSetParent(node,rightChild);
}


static inline void KRbRightRotate(KRbRoot *root,KRbNode *node) {
    if (root == NULL || node == NULL) {
        return ;
    }

    KRbNode *leftChild = node->left;
    node->left = leftChild->right;

    if(leftChild->right != RBTreeNil){
        KRbSetParent(leftChild->right,node);
    }

    KRbSetParent(leftChild,KRbParent(node));
    KRbNode *parent = KRbParent(node);
    if (parent == RBTreeNil) {
        root->node = leftChild;
    } else if(node == parent->right) {
        parent->right = leftChild;
    } else {
        parent->left = leftChild;
    }

    leftChild->right = node;
    KRbSetParent(node,leftChild);
}


KRbNode *KRbSearch(KRbRoot *root,const void *key) {
    if (root == NULL || root->cmp == NULL
        || key == NULL || root->node == RBTreeNil
        || root->cmpKey == NULL) {
        return NULL;
    }

    KRbNode *ptr = root->node;
    int reval = 0;
    while (true) {
        reval = root->cmpKey(ptr,key);
        if (reval == 0) {
            if (root->hit) {
                root->hit(ptr);
            }
            return ptr;
        } else if (reval < 0) {
            if (KRbIsNil(ptr->right)) {
                return NULL;
            } else {
                ptr = ptr->right;
            }
        } else {
            if (KRbIsNil(ptr->left)) {
                return NULL;
            } else {
                ptr = ptr->left;
            }
        }
    }
    return NULL;
}


static inline void KRbInsertFixup(KRbRoot *root,KRbNode *node) {
    KRbNode *pNode = KRbParent(node);
    KRbNode *gpNode = NULL;
    while(KRbIsRed(pNode)) {
        gpNode = KRbParent(pNode);
        if (pNode == gpNode->left) {
            KRbNode *uNode = gpNode->right;
            if (KRbIsRed(uNode)) {
                /* Uppercase means Black
                 * Case 1 - color flips
                 *
                 *       G            g
                 *      / \          / \
                 *     p   u  -->   P   U
                 *    /            /
                 *   n            N
                 */
                KRbSetBlack(pNode);
                KRbSetBlack(uNode);
                KRbSetRed(gpNode);
                node = gpNode;
                pNode = KRbParent(node);
                continue ;
            }

            if (node == pNode->right) {
                /*
                 * Case 2 - left rotate at parent
                 *
                 *      G             G
                 *     / \           / \
                 *    p   U  -->    n   U
                 *     \           /
                 *      n         p
                 *
                 */
                node = pNode;
                KRbLeftRotate(root,node);
                pNode = KRbParent(node);
            }
            /*
             * Case 3 - right rotate at gparent
             *
             *        G           P
             *       / \         / \
             *      p   U  -->  n   g
             *     /                 \
             *    n                   U
             */
            KRbSetBlack(pNode);
            KRbSetRed(gpNode);
            KRbRightRotate(root,gpNode);
        } else {
            KRbNode *uNode = gpNode->left;
            if (KRbIsRed(uNode)) {
                //Case 1
                KRbSetBlack(pNode);
                KRbSetBlack(uNode);
                KRbSetRed(gpNode);
                node = gpNode;
                pNode = KRbParent(node);
                continue ;
            } else if (node == pNode->left) {
                //Case 2
                node = pNode;
                KRbRightRotate(root,node);
                pNode = KRbParent(node);
            }
            //Case 3
            KRbSetBlack(pNode);
            KRbSetRed(gpNode);
            KRbLeftRotate(root,gpNode);
        }
    }
    KRbSetBlack(root->node);
}

int KRbInsertSearched(KRbRoot *root,KRbNode *node,KRbNode *parent) {
    if (root == NULL || root->cmp == NULL
        || node == NULL || parent == NULL) {
        return KE_NULL_PTR;
    }

    if (parent->right != node && parent->left != node) {
        return KE_UNKNOW;
    }

    KRbSetParentColor(node,parent,RBNODE_COLOR_RED);
    node->left = RBTreeNil;
    node->right = RBTreeNil;
    KRbInsertFixup(root,node);
    return KE_OK;
}

int KRbInsert(KRbRoot *root,KRbNode *node) {
    if (root == NULL || root->cmp == NULL
        || node == NULL) {
        return KE_NULL_PTR;
    }

    if (root->node == RBTreeNil) {
        root->node = node;
        node->left = RBTreeNil;
        node->right = RBTreeNil;
        KRbSetParentColor(node,RBTreeNil,RBNODE_COLOR_BLACK);
        return KE_OK;
    }

    KRbNode *ptr = root->node;
    int reval = 0;
    while (true) {
        reval = root->cmp(ptr,node);
        if (reval < 0) {
            if (KRbIsNil(ptr->right)) {
               break;
           } else {
               ptr = ptr->right;
           }
        }
        else {
            if (KRbIsNil(ptr->left)) {
                break;
            } else {
                ptr = ptr->left;
            }
        }
    }

    if (reval < 0) {
        ptr->right = node;
    } else {
        ptr->left = node;
    }

    KRbSetParentColor(node,ptr,RBNODE_COLOR_RED);
    node->left = RBTreeNil;
    node->right = RBTreeNil;
    KRbInsertFixup(root,node);
    return KE_OK;
}

inline static void KRbTransplant(KRbRoot *root,KRbNode *node,KRbNode *child) {
    assert(root);
    assert(node);
    assert(child);

    KRbNode *parent = KRbParent(node);

    if (parent == RBTreeNil) {
        root->node = child;
    } else if (node == parent->left) {
        parent->left = child;
    } else {
        parent->right = child;
    }

    KRbSetParent(child,parent);
}

void KRbDeleteFixup(KRbRoot *root,KRbNode *node) {
    assert(root);
    assert(node);

    while (node != root->node && KRbIsBlack(node)) {
        KRbNode *pNode = KRbParent(node); //It will be invalid after call rotate or something.
        if (node == pNode->left) {
            KRbNode *sNode = pNode->right;
            if (KRbIsRed(sNode)) {
                /*
                 * Case 1 - left rotate at parent
                 *
                 *     P               S
                 *    / \             / \
                 *   N   s    -->    p   SR
                 *      / \         / \
                 *     SL  SR      N   SL
                 */
                KRbSetBlack(sNode);
                KRbSetRed(pNode);
                KRbLeftRotate(root,pNode);
                sNode = KRbParent(node)->right;
            }
            if (KRbIsBlack(sNode->left) && KRbIsBlack(sNode->right)) {
                /*
                 * Case 2 - sibling color flip
                 * (p could be either color here)
                 *
                 *    (p)           (p)
                 *    / \           / \
                 *   N   S    -->  N   s
                 *      / \           / \
                 *     SL  SR        SL  SR
                 *
                 * This leaves us violating 5) which
                 * can be fixed by flipping p to black
                 * if it was red, or by recursing at p.
                 * p is red when coming from Case 1.
                 */
                KRbSetRed(sNode);
                node = KRbParent(node);
            }
            else {
                if (KRbIsBlack(sNode->right)) {
                    /*
                     * Case 3 - right rotate at sibling
                     * (p could be either color here)
                     *
                     *   (p)           (p)
                     *   / \           / \
                     *  N   S    -->  N   SL
                     *     / \             \
                     *    sl  SR            s
                     *                       \
                     *                        SR
                     */
                    KRbSetBlack(sNode->left);
                    KRbSetRed(sNode);
                    KRbRightRotate(root,sNode);
                    sNode = KRbParent(node)->right;
                }
                /*
                 * Case 4 - left rotate at parent + color flips
                 * (p and sl could be either color here.
                 *  After rotation, p becomes black, s acquires
                 *  p's color, and sl keeps its color)
                 *
                 *      (p)             (s)
                 *      / \             / \
                 *     N   S     -->   P   SR
                 *        / \         / \
                 *      (sl) sr      N  (sl)
                 */
                //exchange the color between parent and sibling,and rotate parent lead to save the extra black point in to origin p.that means the blackheight of subtree N has been add 1.so the extra black point released.To maintain the balance of subtree sr,it will be change to black.now the blackheight of Subtree sr not be change.
                KRbSetColor(sNode,KRbColor(KRbParent(node)));
                KRbSetBlack(KRbParent(node));
                KRbSetBlack(sNode->right);
                KRbLeftRotate(root,KRbParent(node));
                node = root->node;
            }
        }
        else
        {
            KRbNode *sNode = pNode->left;
            if (KRbIsRed(sNode)) {
                /*
                 * Case 1 - right rotate at parent
                */
                KRbSetBlack(sNode);
                KRbSetRed(pNode);
                KRbRightRotate(root,pNode);
                sNode = KRbParent(node)->left;
            }
            if (KRbIsBlack(sNode->left) && KRbIsBlack(sNode->right)) {
                /*
                 * Case 2 - sibling color flip
                 * (p could be either color here)
               */
                KRbSetRed(sNode);
                node = KRbParent(node);
            }
            else {
                if (KRbIsBlack(sNode->left)) {
                    /*
                     * Case 3 - left rotate at sibling
                     * (p could be either color here)
                     */
                    KRbSetBlack(sNode->right);
                    KRbSetRed(sNode);
                    KRbLeftRotate(root,sNode);
                    sNode = KRbParent(node)->left;
                }
                /*
                 * Case 4 - rigth rotate at parent + color flips
                */
                KRbSetColor(sNode,KRbColor(KRbParent(node)));
                KRbSetBlack(KRbParent(node));
                KRbSetBlack(sNode->left);
                KRbRightRotate(root,KRbParent(node));
                node = root->node;
            }
        }

    }

    KRbSetBlack(node);
}

void KRbDelete(KRbRoot *root,KRbNode *node) {
    assert(root);
    assert(node);

    int oldColor = KRbColor(node);
    KRbNode *cNode = NULL;

    /*
     * Case 1: node to erase has no more than 1 child (easy!)
     *
     * Note that if there is one child it must be red due to 5)
     * and node must be black due to 4). We adjust colors locally
     */
    if (node->left == RBTreeNil) {
        cNode = node->right;
        KRbTransplant(root,node,node->right);
    } else if (node->right == RBTreeNil) {
        cNode = node->left;
        KRbTransplant(root,node,node->left);
    } else {
        KRbNode *successor= KRbMinimum(node->right);
        oldColor = KRbColor(successor);
        cNode = successor->right;

        if (KRbParent(successor) == node) {
            //there have to ensure that the parent pointer of cNode point to successor, even though cNode might be Nil, fixup need that.
            KRbSetParent(cNode,successor);
        } else {
            /*
             * Case 3: node's successor is leftmost under
             * node's right child subtree
             *
             *    (n)          (s)
             *    / \          / \
             *  (x) (y)  ->  (x) (y)
             *      /            /
             *    (p)          (p)
             *    /            /
             *  (s)          (c)
             *    \
             *    (c)
             */
            KRbTransplant(root,successor,cNode);
            successor->right = node->right;
            KRbSetParent(successor->right,successor);
        }

        /*
         * Case 2: node's successor is its right child
         *
         *    (n)          (s)
         *    / \          / \
         *  (x) (s)  ->  (x) (c)
         *        \
         *        (c)
         */

        KRbTransplant(root,node,successor);
        successor->left = node->left;
        KRbSetParent(successor->left,successor);
        KRbSetColor(successor,KRbColor(node));
    }
    if (oldColor == RBNODE_COLOR_BLACK) {
        //We just lose a unit of blackheight in that subtree. so we need to rebalance it.
        KRbDeleteFixup(root,cNode);
    }
}

int KRbPreOrderTraverse(KRbNode *node,KRbTreeIterativeFun fun,void *param) {
    CheckNULLPtr(node);

    KStack stack;
    KStackInit(&stack);
    while (node != RBTreeNil || !KStackIsEmpty(&stack)) {
        if (node != RBTreeNil) {
            KStackPush(&stack,node);
            node = node->left;
        } else {
            node = (KRbNode *)KStackTop(&stack);
            int reval = fun(node,param);
            if(reval != KE_OK)
            {
                KStackClean(&stack);
                return reval;
            }
            KStackPop(&stack);
            node = node->right;
        }
    }

    return KE_OK;
}


int KRbClearTree(KRbRoot *root,KRbTreeIterativeFun fun,void *param) {
    CheckNULLPtr(root);
    int reval = KE_OK;
    while (KRbNotNil(root->node)) {
        KRbNode *currentNode = root->node;
        KRbDelete(root,root->node);
        if (fun) {
            reval = fun(currentNode,param);
        }

        if (reval < KE_OK) {
            return  reval;
        }
    }
    return KE_OK;
}