#ifndef BTREE_H
#define BTREE_H



#include <vector>
#include <iostream>


template <typename T>
class BTree;

template <typename T>
class BtreeNode{
	public:
	std::vector<T> keys;
	std::vector<BtreeNode*> children;
	bool is_leaf;
	int min_degree;
	


	void insertNonFull(T key);
	void splitChild(int index, BtreeNode* child);
	void traverse();
	void printTree(int level = 0);  // Visualize tree structure
	BtreeNode* search(T key);


	BtreeNode(int degree, bool leaf);
	~BtreeNode();  // ← ADD THIS LINE!
	friend class BTree<T>;

};



template<typename T>
class BTree{
	private: 
		BtreeNode<T>* root;
		int min_degree;
	public:
		BTree(int degree){
			root = nullptr;
			min_degree = degree;
		}
		~BTree();


		void insert(T key);
	    	void printTree();  // Print visual tree structure
		bool search(T key);
		void traverse();

		BtreeNode<T>* getRoot() {return root;}

};



// Add this at the end of b_tree.h, just before #endif

// Helper function to calculate B-tree height
template <typename T>
int calculateBTreeHeight(BtreeNode<T>* node) {
    if (node == nullptr) return 0;
    if (node->is_leaf) return 1;
    
    // All paths have same height in B-tree (balanced property)
    if (node->children.empty()) return 1;
    return 1 + calculateBTreeHeight(node->children[0]);
}




#endif	

