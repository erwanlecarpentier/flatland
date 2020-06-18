#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include <agent.hpp>
#include <environment.hpp>

/**
 * @brief Print grid
 *
 * Print the grid world as a standard output.
 * Environment must be discrete.
 * @param {const environment &} en; displayed environment
 */
void print_grid(const environment &en) {
    assert(!en.is_continuous);
    for(auto & row : en.grid_world) {
        printv(row);
    }
}

/**
 * @brief Print grid with agent's position
 *
 * Print the grid world as a standard output with an X onto the agent's position.
 * Environment must be discrete.
 * @param {const environment &} en; displayed environment
 * @param {const agent &} ag; agent
 */
template <class PLC>
void print_grid_with_agent(const environment &en, const agent<PLC> &ag) {
    assert(!en.is_continuous);
    std::cout << "s:" << ag.s.x << " " << ag.s.y << " ";
    std::cout << "a:" << ag.a.dx << " " << ag.a.dy << " ";
    std::cout << "s_p:" << ag.s_p.x << " " << ag.s_p.y << std::endl;
    unsigned rval = (unsigned) ag.s[0];
    unsigned cval = (unsigned) ag.s[1];
    for(unsigned i=0; i<en.grid_world.size(); ++i) {
        for(unsigned j=0; j<en.grid_world[i].size(); ++j) {
            if(i == rval && j == cval) {
                std::cout << "X ";
            } else {
                if(en.grid_world[i][j] < 0) {
                    std::cout << "- ";
                } else {
                    std::cout << en.grid_world[i][j] << " ";
                }
            }
        }
        std::cout << std::endl;
    }
}

#endif // DISPLAY_HPP_
