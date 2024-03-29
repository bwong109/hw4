#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertHelper(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* curr); 
	void removeHelper(AVLNode<Key, Value>* curr, int diff); 
	void rotateRight(AVLNode<Key, Value>* child); 
	void rotateLeft(AVLNode<Key, Value>* child); 

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item) {

    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* child = nullptr;
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);

    // If root empty, create new node as root
    if (!this->root_) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
        this->root_->setLeft(nullptr);
        this->root_->setRight(nullptr);
        return;
    }

    // Finding appropriate position to insert node
    else {
        while (true) {

            // Update value if key already exists
            if (temp->getKey() == new_item.first) {
                temp->setValue(new_item.second);
                break;
            } 
            
            // If key larger go right
            else if (temp->getKey() > new_item.first) {
                if (!temp->getLeft()) {
                    parent = temp;
                    child = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
                    temp->setLeft(child);
                    parent->updateBalance(-1);

                    // Balance if neccessary
                    if (parent->getBalance() != 0) {
                        insertHelper(parent, child);
                    }
                    break;
                }
                temp = temp->getLeft();
            } 

            // If key is smaller go left
            else if (temp->getKey() < new_item.first) {
                if (!temp->getRight()) {
                    parent = temp;
                    child = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
                    temp->setRight(child);
                    parent->updateBalance(1);

                    // Balance if neccessary
                    if (parent->getBalance() != 0) {
                        insertHelper(parent, child);
                    }
                    break;
                }

                // Move to right child
                temp = temp->getRight();
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    if (!this->root_){
        return;
    }

    int difference;
    AVLNode<Key, Value>* parent;
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));

    // Get parent of node if it exists
    if (node){
        parent = node->getParent();
    }
    else{
        return;
    }

    // Swap node with predecessor if it has two children
    if (node->getLeft() && node->getRight()) {
        nodeSwap(static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(node)), node);
        parent = node->getParent();
    }

    // Calculate difference ni heights between parent and node
    if (parent) {
        difference = (parent->getLeft() != node) ? -1 : 1;
    }

    // Case 1: Node has no children
    if (!node->getLeft() && !node->getRight()) {
        if (this->root_ == node) {
            delete node;
            node = nullptr;
            this->root_ = nullptr;
            return;
        } 
        else {
            // Update if needed removal to null
            if (node == parent->getLeft()){
                parent->setLeft(nullptr);
            }
            else{
                parent->setRight(nullptr);
            }
            delete node;
            node = nullptr;
        }
    } 

    // Case 2: Node has only right children
    else if (!node->getLeft() && node->getRight()) {
        if (this->root_ == node) {
            this->root_ = node->getRight();
            this->root_->setParent(nullptr);
        }
        if (node && parent) {
            // Set parent's left child to node's right
            if (parent->getLeft() == node){
                parent->setLeft(node->getRight());
            }
            // Set parent's right child to node's right
            else{
                parent->setRight(node->getRight());
            }
            node->getRight()->setParent(parent);
        }
        delete node;
        node = nullptr;
    } 

    // Case 3: Node has only left children
    else if (node->getLeft() && !node->getRight()) {
        if (this->root_ == node) {
            this->root_ = this->root_->getLeft();
            this->root_->setParent(nullptr);
        }
        if (node && parent) {
            // Set parent's left child to node's left
            if (parent->getLeft() == node){
                parent->setLeft(node->getLeft());
            }
            // Set parent's right child to node's left
            else{
                parent->setRight(node->getLeft());
            }
            node->getLeft()->setParent(parent);
        }
        delete node;
        node = nullptr;
    }
    removeHelper(parent, difference);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2) {
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertHelper(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) {
    
    // Check if parent or grand parent is null
    if (!parent || !parent->getParent()) {
        return;
    }

    AVLNode<Key, Value>* grandparent = parent->getParent();

    // Check if parent is left child of grandparent
    if (grandparent->getLeft() && grandparent->getLeft() == parent) {
        grandparent->updateBalance(-1);

        // Check if grandparent's balance is -1
        if (grandparent->getBalance() == -1) {
            insertHelper(grandparent, parent);
        } 

        // Check if grandparent's balance is -2
        else if (grandparent->getBalance() == -2) {
            // Check if new node inserted in left subtree
            if (grandparent->getLeft()->getLeft() == node) {
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } 
            // Otherwise insert into right subtree
            else {
                // Update balance factors based on rotations
                rotateLeft(parent);
                rotateRight(grandparent);
                if (node->getBalance() == -1) {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                } 
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                } 
                else if (node->getBalance() == 1) {
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    } 
    // Check if parent is right child of grandparent
    else if (grandparent->getRight() && grandparent->getRight() == parent) {
        grandparent->updateBalance(1);

        // Check if balance factor is 1
        if (grandparent->getBalance() == 1) {
            insertHelper(grandparent, parent);
        } 
        // Check if balance factor is 2
        else if (grandparent->getBalance() == 2) {
            // Check if node inserted into right subtree
            if (grandparent->getRight()->getRight() == node) {
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } 
            // Otherwise, insert into left subtree
            else {
                // Update balance factors as needed
                rotateRight(parent);
                rotateLeft(grandparent);
                if (node->getBalance() == 1) {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                } 
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                } 
                else if (node->getBalance() == -1) {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeHelper(AVLNode<Key, Value>* node, int diff) {
    // Check if the node is null
    if (node == nullptr) {
        return;
    }

    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* child = nullptr;
    int difference = 0;
    parent = node->getParent();
    if (parent) {
        difference = (node->getKey() == parent->getLeft()->getKey()) ? 1 : -1;
    }

    // Check for left rotation balancing
    if (diff == -1) {
        // Case where the node's left subtree is unbalanced
        if (node->getBalance() + diff == -2) {
            child = node->getLeft();

            // Single right rotation if the left child's balance is -1
            if (child->getBalance() == -1) {
                rotateRight(node);
                child->setBalance(0);
                node->setBalance(0);
                removeHelper(parent, difference);
            } 

            // Left-right rotation if the left child's balance is 0
            else if (child->getBalance() == 0) {
                rotateRight(node);
                child->setBalance(1);
                node->setBalance(-1);
                return;
            } 

            // Left-right rotation followed and single right rotation if the left child's balance is 1
            else if (child->getBalance() == 1) {
                AVLNode<Key, Value>* rightChild = child->getRight();
                rotateLeft(child);
                rotateRight(node);
                if (rightChild) {

                    // Adjust balance factors after rotations
                    if (rightChild->getBalance() == 1) {
                        rightChild->setBalance(0);
                        node->setBalance(0);
                        child->setBalance(-1);
                    } 
                    else if (rightChild->getBalance() == 0) {
                        rightChild->setBalance(0);
                        node->setBalance(0);
                        child->setBalance(0);
                    } 
                    else if (rightChild->getBalance() == -1) {
                        rightChild->setBalance(0);
                        node->setBalance(1);
                        child->setBalance(0);
                    }
                }
                removeHelper(parent, difference);
            }
        } 
        // Update balance as needed
        else if (diff + node->getBalance() == -1) {
            node->setBalance(-1);
            return;
        } 
        // Update balance as needed
        else if (diff + node->getBalance() == 0) {
            node->setBalance(0);
            removeHelper(parent, difference);
        }
    } 
    // Check for right rotation balancing
    else if (diff == 1) {
        if (node->getBalance() + diff == 2) {
            AVLNode<Key, Value>* rightChild = node->getRight();

            // Case where the node's right subtree is unbalanced.
            if (rightChild) {

                // Single left rotation if the right child's balance is 1
                if (rightChild->getBalance() == 1) {
                    rotateLeft(node);
                    rightChild->setBalance(0);
                    node->setBalance(0);
                    removeHelper(parent, difference);
                } 

                // Right-left rotation if the right child's balance is 0
                else if (rightChild->getBalance() == 0) {
                    rotateLeft(node);
                    rightChild->setBalance(-1);
                    node->setBalance(1);
                    return;
                } 
                // Right-left rotation and single left rotation if the right child's balance is -1
                else if (rightChild->getBalance() == -1) {
                    AVLNode<Key, Value>* leftGrandchild = rightChild->getLeft();
                    rotateRight(rightChild);
                    rotateLeft(node);
                    if (leftGrandchild->getBalance() == -1) {
                        leftGrandchild->setBalance(0);
                        node->setBalance(0);
                        rightChild->setBalance(1);
                    } 
                    else if (leftGrandchild->getBalance() == 0) {
                        leftGrandchild->setBalance(0);
                        node->setBalance(0);
                        rightChild->setBalance(0);
                    } 
                    else if (leftGrandchild->getBalance() == 1) {
                        leftGrandchild->setBalance(0);
                        node->setBalance(-1);
                        rightChild->setBalance(0);
                    }
                    removeHelper(parent, difference);
                }
            }
        } 
        // Update balance as needed
        else if (node->getBalance() + diff == 1) {
            node->setBalance(1);
            return;
        } 
        // Update balance as needed
        else if (node->getBalance() + diff == 0) {
            node->setBalance(0);
            removeHelper(parent, difference);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* parent = node->getLeft();
    AVLNode<Key, Value>* child = parent->getRight();

    // Adjust parent pointers
    if (!node->getParent()) {
        parent->setParent(nullptr);
        this->root_ = parent;
    } 
    else {
        parent->setParent(node->getParent());
        if (node->getParent()) {
            if (node->getParent()->getLeft() == node) {
                node->getParent()->setLeft(parent);
            } 
            else {
                node->getParent()->setRight(parent);
            }
        }
    }

    // Update child pointers
    node->setLeft(child ? (child->setParent(node), child) : nullptr);

    parent->setRight(node);
    node->setParent(parent);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* parent = node->getRight();
    AVLNode<Key, Value>* child = parent->getLeft();

    // Adjust parent pointers
    if (!node->getParent()) {
        parent->setParent(nullptr);
        this->root_ = parent;
    } 
    else {
        parent->setParent(node->getParent());
        if (node->getParent()) {
            if (node->getParent()->getLeft() == node) {
                node->getParent()->setLeft(parent);
            } 
            else {
                if (node->getParent()->getRight() == node) {
                    node->getParent()->setRight(parent);
                }
            }
        }
    }

    // Update child pointers
    node->setRight(child ? (child->setParent(node), child) : nullptr);

    parent->setLeft(node);
    node->setParent(parent);
}


#endif
