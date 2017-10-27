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
