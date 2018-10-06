
// A disjoint sets implementation using path compression and 
// a weighted union
// ref: https://en.wikipedia.org/wiki/Disjoint-set_data_structure

#include <vector>

class disjoint_sets {

public:

    disjoint_sets(int num_nodes) {

        nodes.resize(num_nodes);

        for (int i = 0; i < num_nodes; i++) {
            nodes[i] = { i, 1 };
        }

    }

    void connect(int node_a, int node_b) {

        int root_a = get_root(node_a);
        int root_b = get_root(node_b);

        if (root_a == root_b) { return; }

        if (nodes[root_a].subtree_size > nodes[root_b].subtree_size) {
            // root_a is bigger so attach B to A
            nodes[root_b].parent = root_a;
            nodes[root_a].subtree_size += nodes[root_b].subtree_size;
        } else {
            // root_b is bigger (or they have the same subtree_size) so attach A to B
            nodes[root_a].parent = root_b;
            nodes[root_b].subtree_size += nodes[root_a].subtree_size;
        }

    }

    bool are_connected(int node_a, int node_b) {

        return get_root(node_a) == get_root(node_b);

    }

    int add_node() {

        int new_node_index = nodes.size();
        nodes.push_back({ new_node_index, 1 });
        return new_node_index;

    }

    void reset() {

        for (int i = 0, l = nodes.size(); i < l; i++) {
            nodes[i].parent = i;
            nodes[i].subtree_size = 1;
        }

    }

    int size() {

        return nodes.size();

    }

private:

    struct node {
        int parent;
        int subtree_size;
    };

    std::vector<node> nodes;

    int get_root(int node) {

        while (node != nodes[node].parent) {
            // flatten tree by setting grandparent of current node 
            // to be it's parent:
            nodes[node].parent = nodes[nodes[node].parent].parent;

            node = nodes[node].parent;
        }

        return node;

    }

};
