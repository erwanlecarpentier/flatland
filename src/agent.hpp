#ifndef AGENT_HPP_
#define AGENT_HPP_

/**
 * @brief Agent class
 *
 * Agent template class.
 */
template<class PLC>
class agent {
public:
    typedef PLC PLC_type;
    PLC policy;
    std::vector<double> state;
    std::vector<double> action;

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {const parameters &} p; parameters
     */
    agent(const parameters &p) : state(p.INITIAL_STATE) {}

    /**
     * @brief Apply policy
     *
     * Modify the action attribute wrt the state attribute and the chosen policy.
     */
    void apply_policy() {
        //
    }
};

#endif // AGENT_HPP_
