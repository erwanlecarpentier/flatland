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
    PLC policy;
    state s; ///< Current state of the agent
    state s_p; ///< Next state of the agent
    std::shared_ptr<action> a; ///< Action selected by the policy
    double reward; ///< Reward from transition (s,a,s_p)

    /**
     * @brief Default constructor
     *
     * Default constructor initialising the parameters via a 'parameters' object.
     * @param {parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction /
     * termination criterion / generative model
     */
    agent(
        parameters &p,
        environment<WRLD> *en) :
        policy(p,en)
    {
        p.parse_state(s);
        s_p = s;
    }

    /**
     * @brief Apply policy
     *
     * Modify the action attribute wrt the state attribute and the chosen policy.
     */
    void apply_policy() {
        a = policy(s);
    }

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     */
    void process_reward() {
        policy.process_reward(s,a,s_p);
    }

    /**
     * @brief Step
     *
     * Go to the next state
     */
    void step() {
        s = s_p;
    }
};

#endif // AGENT_HPP_
