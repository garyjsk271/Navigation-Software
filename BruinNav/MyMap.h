#pragma once

#include <iostream>
#include <queue>

template <typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap(const MyMap &other)          = delete;
    MyMap &operator=(const MyMap &rhs) = delete;

public:
    MyMap() = default;

    ~MyMap() { 
        clear(); 
    }

    // level-order deletion of all nodes.
    void clear()
    {
        auto toBeDeleted = std::queue<node *>{};
        toBeDeleted.push(root_);
        while (!empty(toBeDeleted))
        {
            auto current = toBeDeleted.front();
            if (current != nullptr)
            {
                toBeDeleted.push(current->left);
                toBeDeleted.push(current->right);
            }
            delete current;
            toBeDeleted.pop();
        }
        root_ = nullptr;
        nNodes_ = 0;
    }

    inline int size() const
    {
        return nNodes_;
    }

    void associate(const KeyType &key, const ValueType &value)
    {
        root_ = updateOrInsert(root_, key, value);
    }

    const ValueType *find(const KeyType &key) const
    {
        auto toBeSearched = std::queue<node *>{};
        toBeSearched.push(root_);
        while (!empty(toBeSearched))
        {
            auto current = toBeSearched.front();
            if (current != nullptr)
            {
                if (current->key == key)
                {
                    return &current->value;
                }
                auto nextNode = (key > current->key) ? current->right : current->left;
                toBeSearched.push(nextNode);
            }
            toBeSearched.pop();
        }
        return nullptr;
    }

    ValueType *find(const KeyType &key)
    {
        return const_cast<ValueType *>(const_cast<const MyMap *>(this)->find(key));
    }

private:
    struct node
    {
        node(KeyType theKey, ValueType theValue)
            : key(theKey), value(theValue)
        {
        }

        int getHeight() const 
        { 
            return this == nullptr ? 0 : height; 
        }

        KeyType key;
        ValueType value;
        node *left  = nullptr;
        node *right = nullptr;
        int height  = 0;
    };

private:
    /**
     *  Performs a "left rotation" using oldRoot as the pivot.
     *      B                C
     *     /  \    -->      /  \
     *    A    C           B    D
     *          \         / 
     *           D       A   
     *  @param oldRoot the root of the subtree to be rotated upon.
     *  @return the new root of the subtree.
     */
    node *leftRotate(node *oldRoot)
    {
        auto newRoot    = oldRoot->right;
        oldRoot->right  = newRoot->left;
        newRoot->left   = oldRoot;

        oldRoot->height =
            std::max(oldRoot->left->getHeight(), oldRoot->right->getHeight()) + 1;
        newRoot->height =
            std::max(newRoot->left->getHeight(), newRoot->right->getHeight()) + 1;

        return newRoot;
    }
    /**
     *  Performs a "right rotation" using oldRoot as the pivot.
     *      C             B
     *     /  \    -->   /  \
     *    B    D        A    C
     *   /                    \
     *  A                      D
     *  @param oldRoot the root of the subtree to be rotated upon.
     *  @return the new root of the subtree.
     */
    node *rightRotate(node *oldRoot)
    {
        auto newRoot = oldRoot->left;
        oldRoot->left = newRoot->right;
        newRoot->right = oldRoot;

        oldRoot->height =
            std::max(oldRoot->left->getHeight(), oldRoot->right->getHeight()) + 1;
        newRoot->height =
            std::max(newRoot->left->getHeight(), newRoot->right->getHeight()) + 1;

        return newRoot;
    }

    /**
     *  @param theNode the root of the subtree to be traversed.
     *  @param key the key to be inserted (or updated if already exists)
     *  @param value the value of the key
     *  @return pointer to the inserted (or updated) node.
     *  @post if insertion leads to unbalanced tree, rotations are performed.
     *      Possible rotation cases:
     *        -one right rotation
     *        -one left rotation
     *        -left rotation followed by right rotation
     *        -right rotation followed by left rotation
     */
    node *updateOrInsert(node *theNode, KeyType key, ValueType value)
    {
        if (theNode == nullptr)
        {
            ++nNodes_;
            return new node(key, value);
        }
        if (theNode->key == key)
        {
            theNode->value = value;
            return theNode;
        }

        auto &nextNode      = key < theNode->key ? theNode->left : theNode->right;
        nextNode            = updateOrInsert(nextNode, key, value);
        theNode->height     = 1 + std::max(theNode->left->getHeight(), theNode->right->getHeight());
        auto balanceFactor  = theNode->left->getHeight() - theNode->right->getHeight();

        if (balanceFactor > 1)
        {
            if (theNode->left == nullptr or key < theNode->left->key)
            {
                return rightRotate(theNode);
            }
            theNode->left = leftRotate(theNode->left);
            return rightRotate(theNode);
        }
        if (balanceFactor < -1)
        {
            if (theNode->right == nullptr or key < theNode->right->key)
            {
                theNode->right = rightRotate(theNode->right);
                return leftRotate(theNode);
            }
            return leftRotate(theNode);
        }

        return theNode;
    }

private:
    node *root_ = nullptr;
    int nNodes_ = 0;
};
