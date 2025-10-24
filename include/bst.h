#ifndef BST_H
#define BST_H

#include <iostream>

// Binary Search Tree Node
template <typename T>
class BSTNode {
public:
    T key;
    BSTNode* left;
    BSTNode* right;
    
    BSTNode(T k) : key(k), left(nullptr), right(nullptr) {}
};

// Binary Search Tree
template <typename T>
class BST {
private:
    BSTNode<T>* root;
    
    // Helper functions (recursive)
    BSTNode<T>* insertHelper(BSTNode<T>* node, T key);
    BSTNode<T>* searchHelper(BSTNode<T>* node, T key);
    void traverseHelper(BSTNode<T>* node);
    void printTreeHelper(BSTNode<T>* node, int level);
    void deleteTree(BSTNode<T>* node);
    
public:
    BST() : root(nullptr) {}
    ~BST();
	BSTNode<T>* getRoot() { return root; }
    void insert(T key);
    bool search(T key);
    void traverse();
    void printTree();
};


// Add this at the end of bst.h, just before #endif

// Helper function to calculate BST height
template <typename T>
int calculateBSTHeight(BSTNode<T>* node) {
    if (node == nullptr) return 0;
    
    int leftHeight = calculateBSTHeight(node->left);
    int rightHeight = calculateBSTHeight(node->right);
    
    return 1 + std::max(leftHeight, rightHeight);
}





#endif
