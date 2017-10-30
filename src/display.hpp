#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include <environment.hpp>

/**
 * @brief Print grid
 *
 * Print the grid world as a standard output.
 * Environment must be discrete.
 * @param {environment &} e; displayed environment
 */
void print_grid(environment &e) {
    assert(!e.is_continuous);
    for(auto & row : e.grid_world) {
        printv(row);
    }
}

/**
 * @brief Print grid with agent's position
 *
 * Print the grid world as a standard output with an X onto the agent's position.
 * Environment must be discrete.
 * @param {environment &} e; displayed environment
 * @param {std::vector<double> &} pos; agent's position
 */
void print_grid_with_agent(environment &e, std::vector<double> &pos) {
    assert(!e.is_continuous);
    assert(pos.size() == 2);
    int rval = (int) pos[0];
    int cval = (int) pos[1];
    for(unsigned i=0; i<e.grid_world.size(); ++i) {
        for(unsigned j=0; j<e.grid_world[i].size(); ++j) {
            if(i == rval && j == cval) {
                std::cout << "X ";
            } else {
                if(e.grid_world[i][j] < 0) {
                    std::cout << "- ";
                } else {
                    std::cout << e.grid_world[i][j] << " ";
                }
            }
        }
        std::cout << std::endl;
    }
}

#endif // DISPLAY_HPP_
