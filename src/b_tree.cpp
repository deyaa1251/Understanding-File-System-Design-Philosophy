#include "b_tree.h"



template <typename T>
BtreeNode<T>::BtreeNode(int degree, bool leaf){
	min_degree = degree;
	is_leaf = leaf;
	keys.reserve(2 * min_degree - 1);

	if(!is_leaf){
		children.reserve(2 * min_degree);
	}
}

template <typename T>
BtreeNode<T>::~BtreeNode(){
	for (auto child : children){
		delete child;
	}
}

template <typename T>
BTree<T>::~BTree(){
	if (root != nullptr){
		delete root;
	}
}

template<typename T>
void BtreeNode<T>::traverse(){
	size_t i;

	for(i = 0; i<keys.size(); i++){
		if(!is_leaf){
			children[i]->traverse();
		}
		std::cout << keys[i] << " ";
	}
	if (!is_leaf) {
        children[i]->traverse();
    }	
}


template <typename T>
BtreeNode<T>* BtreeNode<T>::search(T key) {
    // Find the first key greater than or equal to the search key
    size_t i = 0;
    while (i < keys.size() && key > keys[i]) {
        i++;
    }
    
    // If we found the key, return this node
    if (i < keys.size() && keys[i] == key) {
        return this;
    }
    
    // If this is a leaf, key doesn't exist
    if (is_leaf) {
        return nullptr;
    }
    
    // Recursively search in the appropriate child
    return children[i]->search(key);
}

// BTree search: Start from root
template <typename T>
bool BTree<T>::search(T key) {
    return (root == nullptr) ? false : (root->search(key) != nullptr);
}


template <typename T>
void BtreeNode<T>::splitChild(int index, BtreeNode* child) {
    // Create a new node to hold (min_degree-1) keys of child
    BtreeNode* newNode = new BtreeNode(child->min_degree, child->is_leaf);
    
    // Copy the last (min_degree-1) keys from child to newNode
    for (int j = 0; j < min_degree - 1; j++) {
        newNode->keys.push_back(child->keys[j + min_degree]);
    }
    
    // If child is not a leaf, copy the last min_degree children too
    if (!child->is_leaf) {
        for (int j = 0; j < min_degree; j++) {
            newNode->children.push_back(child->children[j + min_degree]);
        }
    }
    
    // The middle key will move up to this node
    T middleKey = child->keys[min_degree - 1];
    
    // Reduce child's size
    child->keys.resize(min_degree - 1);
    if (!child->is_leaf) {
        child->children.resize(min_degree);
    }
    
    // Insert newNode as a child of this node
    children.insert(children.begin() + index + 1, newNode);
    
    // Insert the middle key into this node
    keys.insert(keys.begin() + index, middleKey);
}









template <typename T>
void BtreeNode<T>::insertNonFull(T key) {
    size_t i = keys.size() - 1;  // Start from rightmost key
    
    if (is_leaf) {
        // This is a leaf node - insert directly
        keys.push_back(T());  // Make space
        
        // Shift keys to make room for new key
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            i--;
        }
        
        // Insert the key
        keys[i + 1] = key;
    } 
    else {
        // This is an internal node - find which child to insert into
        
        // Find the child where key should go
        while (i >= 0 && keys[i] > key) {
            i--;
        }
        i++;  // Now children[i] is the correct child
        
        // Check if the child is full
        if (children[i]->keys.size() == (2 * min_degree - 1)) {
            // Child is full, split it first
            splitChild(i, children[i]);
            
            // After split, decide which of the two children to use
            if (keys[i] < key) {
                i++;
            }
        }
        
        // Recursively insert into the appropriate child
        children[i]->insertNonFull(key);
    }
}

template <typename T>
void BTree<T>::insert(T key) {
    // Case 1: Tree is empty
    if (root == nullptr) {
        root = new BtreeNode<T>(min_degree, true);
        root->keys.push_back(key);
        return;
    }
    
    // Case 2: Root is full - need to split root
    if (root->keys.size() == (2 * min_degree - 1)) {
        // Create new root
        BtreeNode<T>* newRoot = new BtreeNode<T>(min_degree, false);
        
        // Old root becomes child of new root
        newRoot->children.push_back(root);
        
        // Split the old root
        newRoot->splitChild(0, root);
        
        // New root has 2 children now, decide which one gets the key
        size_t i = 0;
        if (newRoot->keys[0] < key) {
            i++;
        }
        newRoot->children[i]->insertNonFull(key);
        
        // Update root
        root = newRoot;
    }
    // Case 3: Root is not full
    else {
        root->insertNonFull(key);
    }
}





template <typename T>
void BTree<T>::traverse() {
    if (root != nullptr) {
        root->traverse();
    }
    std::cout << std::endl;
}




// Print tree structure with indentation showing levels
template <typename T>
void BtreeNode<T>::printTree(int level) {
    // Print current node with indentation
    std::cout << std::string(level * 4, ' ');  // 4 spaces per level
    
    std::cout << "[";
    for (size_t i = 0; i < keys.size(); i++) {
        std::cout << keys[i];
        if (i < keys.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
    
    if (is_leaf) {
        std::cout << " (leaf)";
    }
    std::cout << std::endl;
    
    // Recursively print children
    if (!is_leaf) {
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->printTree(level + 1);
        }
    }
}

// BTree wrapper for printTree
template <typename T>
void BTree<T>::printTree() {
    if (root == nullptr) {
        std::cout << "(empty tree)" << std::endl;
        return;
    }
    
    std::cout << "\n=== Tree Structure ===" << std::endl;
    root->printTree(0);
    std::cout << "=====================\n" << std::endl;
}


template class BTree<int>;
template class BtreeNode<int>;
