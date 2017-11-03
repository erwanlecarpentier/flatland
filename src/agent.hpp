#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <environment.hpp>

/**
 * @brief Agent class
 *
 * Agent template class.
 */
template <class PLC, class WRLD>
class agent {
public:
    typedef PLC PLC_type;
    //typedef WRLD WRLD_type;
    PLC policy;
    std::vector<double> state; ///< Current state of the agent
    std::vector<double> action; ///< Action selected by the policy
    std::vector<double> state_p; ///< Next state of the agent
    double reward; ///< Reward from transition (state,action,state_p)

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction /
     * termination criterion / generative model
     */
    agent(
        const parameters &p,
        environment<WRLD> *en) :
        policy(p,en),
        state(p.INITIAL_STATE)
    {
        state_p = state;
        action = std::vector<double> {0.,0.};
    }

    /**
     * @brief Apply policy
     *
     * Modify the action attribute wrt the state attribute and the chosen policy.
     */
    void apply_policy() {
        action = policy(state);
    }

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (state,action,state_p)
     */
    void process_reward() {
        policy.process_reward(state,action,state_p);
    }

    /**
     * @brief Step
     *
     * Go to the next state
     */
    void step() {
        state = state_p;
    }
};

#endif // AGENT_HPP_
