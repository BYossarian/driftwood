
#include <iostream>
#include <cmath>

// an augmented binary search tree implementation of a Set ADT

// TODO: give each node a parent link so that when a node with two children is 
// removed, we can actually remove the node with that value rather than just changing
// it's value (thus making nodes immutable)
// TODO: successor etc. using parent


template <typename T>
class binary_search_tree {

public:

    binary_search_tree(): root(nullptr) {}
    ~binary_search_tree() {

        delete root;

    }

    // inserts value into the tree if it doesn't already exist
    // returns whether or not a new value was inserted
    bool insert(T value) {

        if (root == nullptr) {
            root = new node(value);
            return true;
        }

        return insert_into_subtree(root, value);

    }

    // removes value, if it exists within the tree (returns whether or not the value
    // existed)
    bool remove(T value) {

        if (root == nullptr) {
            return false;
        }

        return remove_from_subtree(root, value);

    }

    // returns the number of items <= value
    int rank(T value) const {

        if (root == nullptr) {
            return 0;
        }

        return subtree_count(root, value);
    }

    bool contains(T value) const {

        if (root == nullptr) {
            return false;
        }

        return subtree_contains(root, value);

    }

    T find_min() const {

        if (root == nullptr) {
            throw;
        }

        return find_subtree_min(root);

    }

    T find_max() const {

        if (root == nullptr) {
            throw;
        }

        return find_subtree_max(root);

    }

    int size() const {
        if (root == nullptr) {
            return 0;
        }
        return root->subtree_size;
    }

    // verify that structure is a BST and that subtree_size properties
    // are correct (used for testing):
    bool verify() const {
        if (root == nullptr) {
            return true;
        }

        return verify_subtree(root);

    }

    // just prints out the contained values in ascending order
    void sorted_print() const {

        if (root == nullptr) {
            std::cout << "-\n";
            return;
        }

        print_subtree(root);

        std::cout << "\n";

    }

    binary_search_tree(const binary_search_tree&) = delete;
    binary_search_tree& operator=(const binary_search_tree&) = delete;

private:

    struct node {

        node(T value): value(value), left(nullptr), right(nullptr), subtree_size(1) {}
        ~node() {
            delete left;
            delete right;
        }
        
        T value;
        node* left;
        node* right;
        int subtree_size;

    };

    node* root;

    bool insert_into_subtree(node* &subtree_root, T value) {

        if (subtree_root->value == value) {
            return false;
        }

        bool inserted = false;

        if (subtree_root->value > value) {
            if (subtree_root->left != nullptr) {
                inserted = insert_into_subtree(subtree_root->left, value);
            } else {
                subtree_root->left = new node(value);
                inserted = true;
            }            
        } else {
            if (subtree_root->right != nullptr) {
                inserted = insert_into_subtree(subtree_root->right, value);    
            } else {
                subtree_root->right = new node(value);
                inserted = true;
            }
        }

        if (inserted) {
            subtree_root->subtree_size++;
        }

        return inserted;

    }

    bool remove_from_subtree(node* &subtree_root, T value) {

        bool removed = false;

        if (subtree_root->value == value) {
            // ok, need to remove this node:

            if (subtree_root->left == nullptr) {
                if (subtree_root->right == nullptr) {
                    // subtree_root has no children
                    delete subtree_root;
                    subtree_root = nullptr;
                    return true;
                }

                // subtree_root has only right child
                node* temp = subtree_root;
                subtree_root = subtree_root->right;
                // need to disconnect node we're about to delete from 
                // it's children due to node destructor's calling delete
                // recursively on children:
                temp->right = nullptr;
                delete temp;

                return true;

            } else if (subtree_root->right == nullptr) {

                // subtree_root has only left child
                node* temp = subtree_root;
                subtree_root = subtree_root->left;
                // need to disconnect node we're about to delete from 
                // it's children due to node destructor's calling delete
                // recursively on children:
                temp->left = nullptr;
                delete temp;

                return true;

            } else {
                // node has two children:

                // swap out current value for smallest value in right subtree
                // and then remove that value from the right subtree:
                T value = find_subtree_min(subtree_root->right);
                subtree_root->value = value;
                removed = remove_from_subtree(subtree_root->right, value);
            }

        } else if (subtree_root->value > value) {
            if (subtree_root->left == nullptr) {
                return false;
            }
            removed = remove_from_subtree(subtree_root->left, value);
        } else {
            if (subtree_root->right == nullptr) {
                return false;
            }
            removed = remove_from_subtree(subtree_root->right, value);
        }

        if (removed) {
            subtree_root->subtree_size--;
        }

        return removed;

    }

    int subtree_count(node* subtree_root, T value) const {

        if (subtree_root->value > value) {
            if (subtree_root->left != nullptr) {
                return subtree_count(subtree_root->left, value);
            }
            return 0;
        } else {

            int count = 1;

            if (subtree_root->left != nullptr) {
                count += subtree_root->left->subtree_size;
            }
            if (subtree_root->right != nullptr) {
                count += subtree_count(subtree_root->right, value);
            }

            return count;

        }

    }

    bool subtree_contains(node* subtree_root, T value) const {

        if (subtree_root->value == value) {
            return true;
        }

        if (subtree_root->value > value) {
            if (subtree_root->left != nullptr) {
                return subtree_contains(subtree_root->left, value);
            }
            return false;        
        } else {
            if (subtree_root->right != nullptr) {
                return subtree_contains(subtree_root->right, value);    
            }
            return false;
        }

    }

    T find_subtree_min(node* current_node) const {

        while (current_node->left != nullptr) {
            current_node = current_node->left;
        }

        return current_node->value;

    }

    T find_subtree_max(node* current_node) const {

        while (current_node->right != nullptr) {
            current_node = current_node->right;
        }

        return current_node->value;

    }

    bool verify_subtree(node* subtree_root) const {

        // post-order traversal (therefore can check properies 
        // recursively from the group up):
        if (subtree_root->left != nullptr) {
            bool valid = verify_subtree(subtree_root->left);
            if (!valid) { return false; }
        }
        if (subtree_root->right != nullptr) {
            bool valid = verify_subtree(subtree_root->right);
            if (!valid) { return false; }
        }

        // check BST invariant:
        if (subtree_root->left != nullptr) {
            int left_subtree_max = find_subtree_max(subtree_root->left);
            if (left_subtree_max >= subtree_root->value) {
                return false;
            }
        }
        if (subtree_root->right != nullptr) {
            int right_subtree_min = find_subtree_min(subtree_root->right);
            if (right_subtree_min <= subtree_root->value) {
                return false;
            }
        }
        
        // check subtree_size property
        int left_subtree_size = (subtree_root->left == nullptr) ? 0 : subtree_root->left->subtree_size;
        int right_subtree_size = (subtree_root->right == nullptr) ? 0 : subtree_root->right->subtree_size;
        if (subtree_root->subtree_size != 1 + left_subtree_size + right_subtree_size) { return false; }

        return true;

    }

    void print_subtree(node* subtree_root) const {

        if (subtree_root->left != nullptr) {
            print_subtree(subtree_root->left);
        }

        std::cout << subtree_root->value << " (" << subtree_root->subtree_size << ") ";

        if (subtree_root->right != nullptr) {
            print_subtree(subtree_root->right);
        }

    }

};
