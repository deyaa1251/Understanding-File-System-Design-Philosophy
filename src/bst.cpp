#include "bst.h"
#include <string>

// Destructor - delete all nodes
template <typename T>
BST<T>::~BST() {
    deleteTree(root);
}

template <typename T>
void BST<T>::deleteTree(BSTNode<T>* node) {
    if (node == nullptr) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

// Insert a key into the BST
template <typename T>
void BST<T>::insert(T key) {
    root = insertHelper(root, key);
}

template <typename T>
BSTNode<T>* BST<T>::insertHelper(BSTNode<T>* node, T key) {
    // Base case: empty tree or reached a leaf position
    if (node == nullptr) {
        return new BSTNode<T>(key);
    }
    
    // Recursive case: go left or right
    if (key < node->key) {
        node->left = insertHelper(node->left, key);
    } else if (key > node->key) {
        node->right = insertHelper(node->right, key);
    }
    // If key == node->key, don't insert duplicates
    
    return node;
}

// Search for a key
template <typename T>
bool BST<T>::search(T key) {
    return searchHelper(root, key) != nullptr;
}

template <typename T>
BSTNode<T>* BST<T>::searchHelper(BSTNode<T>* node, T key) {
    // Base case: empty or found
    if (node == nullptr || node->key == key) {
        return node;
    }
    
    // Recursive case
    if (key < node->key) {
        return searchHelper(node->left, key);
    } else {
        return searchHelper(node->right, key);
    }
}

// In-order traversal (prints in sorted order)
template <typename T>
void BST<T>::traverse() {
    traverseHelper(root);
    std::cout << std::endl;
}

template <typename T>
void BST<T>::traverseHelper(BSTNode<T>* node) {
    if (node == nullptr) return;
    
    traverseHelper(node->left);
    std::cout << node->key << " ";
    traverseHelper(node->right);
}

// Print tree structure
template <typename T>
void BST<T>::printTree() {
    if (root == nullptr) {
        std::cout << "(empty tree)" << std::endl;
        return;
    }
    
    std::cout << "\n=== BST Structure ===" << std::endl;
    printTreeHelper(root, 0);
    std::cout << "====================\n" << std::endl;
}

template <typename T>
void BST<T>::printTreeHelper(BSTNode<T>* node, int level) {
    if (node == nullptr) return;
    
    // Print right subtree first (top of visual tree)
    printTreeHelper(node->right, level + 1);
    
    // Print current node with indentation
    std::cout << std::string(level * 4, ' ') << "[" << node->key << "]" << std::endl;
    
    // Print left subtree (bottom of visual tree)
    printTreeHelper(node->left, level + 1);
}

// Template instantiation
template class BST<int>;
template class BSTNode<int>;
