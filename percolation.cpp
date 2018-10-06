
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>

#include "./disjoint-sets/disjoint-sets.h"
#include "./helpers/weighted-coin.h"

void randomise_sites(std::vector<bool> &grid, double open_site_probability) {

    weighted_coin coin(open_site_probability);
    int gridSize = grid.size();
    for (int i = 0; i < gridSize; i++) {
        grid[i] = coin.toss();
    }

}

void make_connections(const std::vector<bool> &grid, disjoint_sets &connections, int grid_side_length) {

    connections.reset();

    for (int i = 0; i < grid_side_length; i++) {
        // connect top element to top row:
        connections.connect(0, i + 2);
        // connect bottom element to bottom row:
        connections.connect(1, grid_side_length * grid_side_length + 1 - i);
    }

    // add in connections due to adjacent open sites:
    // NB: due to symmetry, we only need to check to the right 
    // and below each site to see if they are open:
    for (int y = 0; y < grid_side_length; y++) {
        for (int x = 0; x < grid_side_length; x++) {

            int current_grid_index = y * grid_side_length + x;

            // current site isn't open, so doesn't connect to anything
            if (!grid[current_grid_index]) { continue; }

            int right_grid_index = current_grid_index + 1;
            if (grid[right_grid_index] && x != grid_side_length - 1) {
                // site to the right is open so make a connection:
                // NB: adding 2 to the indices because the connection nodes contain 
                // the top and bottom elements at 0 and 1:
                connections.connect(current_grid_index + 2, right_grid_index + 2);
            }

            int bottom_grid_index = current_grid_index + grid_side_length;
            if (grid[bottom_grid_index] && y != grid_side_length - 1) {
                // site below is open so make a connection:
                // NB: adding 2 to the indices because the connection nodes contain 
                // the top and bottom elements at 0 and 1:
                connections.connect(current_grid_index + 2, bottom_grid_index + 2);
            }

        }
    }

}

int main() {

    // using disjoint-sets to model percolation.
    // basically, we have a grid of cells that can be open or closed, as well 
    // a top and bottom that are connected to every cell in the top/bottom rows 
    // respectively. a grid percolates if the top and bottom are connected through 
    // a path of open cells (paths can't move diagonally - just up/down, left/right).
    // we're looking at the probability that a grid percolates given the 
    // probability that any one site is open:

    int grid_side_length = 1000;
    int num_trials = 25;

    std::vector<bool> grid(grid_side_length * grid_side_length);

    // nodes 0 and 1 will represent top and bottom elements that connect to 
    // the sites in the top/bottom rows respectively:
    disjoint_sets connections(grid_side_length * grid_side_length + 2);

    // we know there's a phase transition between open_site_probability = 0.585 -> 0.6 
    // such that the probability of percolation goes from 0 to 1 very quickly:
    for (double open_site_probability = 0.585; open_site_probability <= 0.6; open_site_probability += 0.001) {

        int successful_trials = 0;

        for (int i = 0; i < num_trials; i++) {
            // randomly open sites within the grid:
            randomise_sites(grid, open_site_probability);
            // use a disjoint sets data structure to model the paths of open sites 
            // through the grid (i.e. we connect neighbouring sites when they're both open)
            make_connections(grid, connections, grid_side_length);
            // now just test if the top/bottom are connected:
            if (connections.are_connected(0, 1)) { successful_trials++; }
        }

        std::cout << open_site_probability << ", " << successful_trials << "\n";

    }

}
