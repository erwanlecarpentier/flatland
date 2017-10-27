#ifndef AGENT_HPP_
#define AGENT_HPP_

struct agent {
    std::vector<double>state;

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     */
    agent(parameters p) {
        state = p.INITIAL_STATE;
    }
};

#endif // AGENT_HPP_
