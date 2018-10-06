
#include <iostream>
#include <cmath>

// an AVL tree implementation of a Set

template <typename T>
class avl_tree {

public:

    avl_tree(): root(nullptr) {}
    ~avl_tree() {

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

    int height() const {
        if (root == nullptr) {
            return 0;
        }
        return root->height;
    }

    // verify that structure is a BST that obeys the 
    // AVL property, and that height and subtree_size properties
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

    avl_tree(const avl_tree&) = delete;
    avl_tree& operator=(const avl_tree&) = delete;

private:

    struct node {

        node(T value): value(value), left(nullptr), right(nullptr), subtree_size(1), height(0) {}
        ~node() {
            delete left;
            delete right;
        }
        
        T value;
        node* left;
        node* right;
        int subtree_size;
        int height;

    };

    node* root;

    // calculate height of a node based on the height of it's children:
    int calculate_height(node* &node) const {

        int left_child_height = (node->left == nullptr) ? -1 : node->left->height;
        int right_child_height = (node->right == nullptr) ? -1 : node->right->height;
        return std::max(left_child_height, right_child_height) + 1;

    }

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
            subtree_root->height = calculate_height(subtree_root);
            maintain_avl_invariant(subtree_root);
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
            subtree_root->height = calculate_height(subtree_root);
            maintain_avl_invariant(subtree_root);
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

    // rotate a subtree one place to the left:
    void rotate_left(node* &subtree_root) {

        if (subtree_root->right == nullptr) {
            // can't rotate left if there's nothing on the right, 
            // and rotate_left shouldn't be getting called in that case:
            throw;
        }

        // first update the structure:
        node* old_subtree_root = subtree_root;
        subtree_root = subtree_root->right;
        old_subtree_root->right = subtree_root->left;
        subtree_root->left = old_subtree_root;

        // now sort subtree_size and height:
        subtree_root->subtree_size = old_subtree_root->subtree_size;
        old_subtree_root->subtree_size = 1 + (old_subtree_root->left == nullptr ? 0 : old_subtree_root->left->subtree_size) 
                                            + (old_subtree_root->right == nullptr ? 0 : old_subtree_root->right->subtree_size);

        old_subtree_root->height = calculate_height(old_subtree_root);
        subtree_root->height = calculate_height(subtree_root);

    }

    // rotate a subtree one place to the right:
    void rotate_right(node* &subtree_root) {

        if (subtree_root->left == nullptr) {
            throw;
        }

        // first update the structure:
        node* old_subtree_root = subtree_root;
        subtree_root = subtree_root->left;
        old_subtree_root->left = subtree_root->right;
        subtree_root->right = old_subtree_root;

        // now sort subtree_size and height:
        subtree_root->subtree_size = old_subtree_root->subtree_size;
        old_subtree_root->subtree_size = 1 + (old_subtree_root->left == nullptr ? 0 : old_subtree_root->left->subtree_size) 
                                            + (old_subtree_root->right == nullptr ? 0 : old_subtree_root->right->subtree_size);

        old_subtree_root->height = calculate_height(old_subtree_root);
        subtree_root->height = calculate_height(subtree_root);

    }

    // assumes heights of child nodes are correct:
    int calc_balance_factor(node* &node) const {
        int left_child_height = (node->left == nullptr) ? -1 : node->left->height;
        int right_child_height = (node->right == nullptr) ? -1 : node->right->height;
        return right_child_height - left_child_height;
    }

    // checks the avl invariant for some node, and corrects it's subtree if needed
    // (assumes that all nodes in the subtree other than the root are already 
    // satisfying the invariant):
    void maintain_avl_invariant(node* &subtree_root) {

        int balance_factor = calc_balance_factor(subtree_root);

        if (std::abs(balance_factor) <= 1) {
            // AVL invariant is satisfied so no need to do anything:
            return;
        }

        // this motherfucker is in violation, and is about to get it's 
        // punk-ass rotated into shape...

        if (balance_factor > 0) {
            // right subtree is taller than left

            int right_child_balance_factor = calc_balance_factor(subtree_root->right);

            if (right_child_balance_factor >= 0) {
                rotate_left(subtree_root);
            } else {
                rotate_right(subtree_root->right);
                rotate_left(subtree_root);
            }

        } else {
            // left subtree is taller than right

            int left_child_balance_factor = calc_balance_factor(subtree_root->left);

            if (left_child_balance_factor <= 0) {
                rotate_right(subtree_root);
            } else {
                rotate_left(subtree_root->left);
                rotate_right(subtree_root);
            }

        }

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

        // check AVL invariant:
        if (std::abs(calc_balance_factor(subtree_root)) > 1) {
            return false;
        }
        
        // check subtree_size property
        int left_subtree_size = (subtree_root->left == nullptr) ? 0 : subtree_root->left->subtree_size;
        int right_subtree_size = (subtree_root->right == nullptr) ? 0 : subtree_root->right->subtree_size;
        if (subtree_root->subtree_size != 1 + left_subtree_size + right_subtree_size) { return false; }

        // check height property
        if (subtree_root->height != calculate_height(subtree_root)) { return false; }

        return true;

    }

    void print_subtree(node* subtree_root) const {

        if (subtree_root->left != nullptr) {
            print_subtree(subtree_root->left);
        }

        std::cout << subtree_root->value << " (" << subtree_root->subtree_size << ", " << subtree_root->height << ") ";

        if (subtree_root->right != nullptr) {
            print_subtree(subtree_root->right);
        }

    }

};
