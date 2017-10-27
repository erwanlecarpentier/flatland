#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

struct environment {
    bool is_continuous;
    std::vector<std::vector<int>> grid_world;

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     */
    environment(parameters p) {
        is_continuous = p.IS_CONTINUOUS;
        if(is_continuous) {
            std::cout << "TODO: implement continuous world" << std::endl;
        } else {
            grid_world = p.GRID_WORLD;
        }
    }

    /**
     * @brief Grid world value
     *
     * Evaluate the grid world at the given state.
     * @param {const std::vector<double> &} s; given state
     * @return Return the value of the grid.
     */
    int grid_world_value(const std::vector<double> &s) {
        assert(s.size() == 2);
        return grid_world.at(s.at(0)).at(s.at(1));
    }

    /**
     * @brief Is terminal
     *
     * Test if the given state is terminal.
     * Convention for gridworld if 1 for a terminal grid cell.
     * @param {const std::vector<double> &} s; given state
     * @return Return true if the test is terminal, else false.
     */
    bool is_terminal(const std::vector<double> &s) {
        if(is_continuous) {
            std::cout << "TODO: implement continuous world" << std::endl;
            return true;
        } else {
            if(grid_world_value(s) == 1) {
                return true;
            } else {
                return false;
            }
        }
    }

    /**
     * @brief Print grid
     *
     * Print the grid world as a standard output.
     * Environment must be discrete.
     */
    void print_grid() {
        assert(!is_continuous);
        for(auto & row : grid_world) {
            printv(row);
        }
    }
};

#endif // ENVIRONMENT_HPP_
