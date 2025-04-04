#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// Check if all leaf paths are equal
bool checkPaths(Node* node, int depth, int& leafDepth) {
    if (node == nullptr) {
        return true;
    }

    // If it is a leaf
    if (node->left == nullptr && node->right == nullptr) {
        if (leafDepth == -1) {
            leafDepth = depth;
        }
        return depth == leafDepth;
    }

    // Recursive check left and right subtrees
    bool leftOK = checkPaths(node->left, depth + 1, leafDepth);
    bool rightOK = checkPaths(node->right, depth + 1, leafDepth);
    return leftOK && rightOK;
}


bool equalPaths(Node * root)
{
    int leafDepth = -1;
    return checkPaths(root, 0, leafDepth);
}

