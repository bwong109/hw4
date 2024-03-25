#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

int height (Node *root){
    // Check if null
    if (root == nullptr){
        return 0;
    }
    else{
        // Get height of left and right
        int leftHeight = height(root -> left);
        int rightHeight = height(root -> right);

        // Return the longer height 
        if (leftHeight > rightHeight) {
            return leftHeight + 1;
        }
        else {
            return rightHeight + 1;
        }
    }

}

bool equalPaths(Node * root)
{
    // Check if root is null or leaf node
    if (root == nullptr || (root->left == nullptr && root->right == nullptr)) {
        return true;
    }

    // Check if only one subtree is present
    if (root->left == nullptr) {
        return height(root->right) == 1;
    }
    if (root->right == nullptr) {
        return height(root->left) == 1;
    }

    // Check if paths are of equal length
    if (height(root->left) == height(root->right)){
        return true;
    }
    else{
        return false;
    }
}


