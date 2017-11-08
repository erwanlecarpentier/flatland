#ifndef OLUCT_POLICY_HPP_
#define OLUCT_POLICY_HPP_

#include <environment.hpp>
#include <uct.hpp>

/**
 * @brief OLUCT policy
 */
class oluct {
public:
    unsigned decision_criterion_selector;
    environment * envt; ///< Pointer to an environment, used for action space reduction
    uct pl;

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction
     */
    oluct(const parameters &p, environment *en) :
        pl(p,en)
    {
        decision_criterion_selector = p.DECISION_CRITERION_SELECTOR;
    }

    /**
     * @brief Switch on decision criterion
     *
     * Select the decision criterion according to the chosen algorithm.
     * The tested tree is the current tree saved in the parameters.
     * @param {const state &} s; the current state of the agent
     * @return Return 'true' if the sub-tree is kept.
     */
    bool decision_criterion(const state & s) {
        (void) s;
        switch(decision_criterion_selector) {
            default: { // Plain decision criterion
                return true;
            }
        }
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; given state
     * @return Return the undertaken action at s.
     */
	std::shared_ptr<action> operator()(const state &s) {
        if(!pl.root_node.is_fully_expanded() || !decision_criterion(s)) {
            pl.build_uct_tree(s);
        }
        std::shared_ptr<action> ra = pl.get_recommended_action(pl.root_node);
        pl.root_node.move_to_child(pl.argmax_score(pl.root_node),s);
        return ra;
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     * @param {state &} s; state
     * @param {std::shared_ptr<action> &} a; action
     * @param {state &} s_p; next state
     */
    void process_reward(
        const state & s,
        const std::shared_ptr<action> & a,
        const state & s_p)
    {
        /* OLUCT policy does not learn. */
        (void) s;
        (void) a;
        (void) s_p;
    }

    /**
     * @brief Get backup
     *
     * Get the backed-up values.
     * @return Return a vector containing the values to be saved.
     */
    std::vector<double> get_backup() {
        return pl.get_backup();
    }
};

#endif // OLUCT_POLICY_HPP_
