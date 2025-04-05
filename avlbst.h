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
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rebalance(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (curr != nullptr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = static_cast<AVLNode<Key, Value>*>(curr->getLeft());
        }
        else if (new_item.first > curr->getKey()) {
            curr = static_cast<AVLNode<Key, Value>*>(curr->getRight());
        }
        else {
            curr->setValue(new_item.second);
            return;
        }
    }
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    AVLNode<Key, Value>* child = newNode;
    AVLNode<Key, Value>* anc = parent;
    while (anc != nullptr) {
        if (child == static_cast<AVLNode<Key, Value>*>(anc->getLeft())) {
            anc->updateBalance(-1);
        }
        else {
            anc->updateBalance(1);
        }

        int8_t bal = anc->getBalance();
        if (bal ==0) break;
        else if (bal == -2 || bal == 2) {
            rebalance(anc);
            break;
        }
        child = anc;
        anc = static_cast<AVLNode<Key, Value>*>(anc->getParent());
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* target = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (target == nullptr) return;

    AVLNode<Key, Value>* nodeToRebalance = nullptr;
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(target->getParent());

    if (target->getLeft() != nullptr && target->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(target));
        this->nodeSwap(target, pred);
    }

    AVLNode<Key, Value>* child = (target->getLeft() != nullptr) ? static_cast<AVLNode<Key, Value>*>(target->getLeft()) : static_cast<AVLNode<Key, Value>*>(target->getRight());
    parent = static_cast<AVLNode<Key, Value>*>(target->getParent());
    if (child != nullptr) {
        child->setParent(parent);
    }
    if (parent == nullptr) {
        this->root_ = child;
        nodeToRebalance = nullptr;
    }
    else {
        if (target == parent->getLeft()) {
            parent->setLeft(child);
            parent->updateBalance(1);
            nodeToRebalance = parent;
        }
        else {
            parent->setRight(child);
            parent->updateBalance(-1);
            nodeToRebalance = parent;
        }
    }
    delete target;

    AVLNode<Key, Value>* current = nodeToRebalance;
    while (current != nullptr) {
        int8_t bal = current->getBalance();
        if (bal == 2 || bal == -2) {
            rebalance(current);
        }
        AVLNode<Key, Value>* parentNode = static_cast<AVLNode<Key, Value>*>(current->getParent());
        if (parentNode != nullptr) {
            int parentOld = parentNode->getBalance();
            if (current == parentNode->getLeft()) {
                parentNode->updateBalance(1);
            }
            else {
                parentNode->updateBalance(-1);
            }
            if (parentOld == 0) break;
        }
        current = parentNode;
    }
}



// Rotate left helper
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* rightChild = node->getRight();
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr) {
        rightChild->getLeft()->setParent(node);
    }
    rightChild->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = rightChild;
    }
    else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(rightChild);
    }
    else {
        node->getParent()->setRight(rightChild);
    }
    rightChild->setLeft(node);
    node->setParent(rightChild);

    int8_t rBal = rightChild->getBalance();
    node->setBalance(node->getBalance() - 1 - std::max(rBal, (int8_t)0));
    rightChild->setBalance(rightChild->getBalance() - 1 + std::min(node->getBalance(), (int8_t)0));
}


// Rotate right helper
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* leftChild = node->getLeft();
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr) {
        leftChild->getRight()->setParent(node);
    }
    leftChild->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = leftChild;
    }
    else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(leftChild);
    }
    else {
        node->getParent()->setRight(leftChild);
    }
    leftChild->setRight(node);
    node->setParent(leftChild);

    int8_t lBal = leftChild->getBalance();
    node->setBalance(node->getBalance() + 1 - std::min(lBal, (int8_t)0));
    leftChild->setBalance(leftChild->getBalance() + 1 + std::max(node->getBalance(), (int8_t)0));
}

// Rebalance helper

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node) {
    if (node->getBalance() == -2) {
        AVLNode<Key, Value>* leftChild = node->getLeft();
        if (leftChild->getBalance() <= 0) {
            rotateRight(node);
        }
        else {
            rotateLeft(leftChild);
            rotateRight(node);
        }
    }
    else if (node->getBalance() == 2) {
        AVLNode<Key, Value>* rightChild = node->getRight();
        if (rightChild->getBalance() >= 0) {
            rotateLeft(node);
        }
        else {
            rotateRight(rightChild);
            rotateLeft(node);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
