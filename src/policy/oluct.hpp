#ifndef OLUCT_POLICY_HPP_
#define OLUCT_POLICY_HPP_

#include <environment.hpp>
#include <uct.hpp>

/**
 * @brief OLUCT policy
 */
template <class WRLD>
class oluct {
public:
    std::vector<std::vector<double>> action_space; ///< Full action space
    unsigned decision_criterion_selector;
    environment<WRLD> * envt; ///< Pointer to an environment, used for action space reduction
    uct<WRLD> pl;

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {const parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction
     */
    oluct(const parameters &p, environment<WRLD> *en) :
        pl(p,en)
    {
        action_space = p.ACTION_SPACE;
        decision_criterion_selector = p.DECISION_CRITERION_SELECTOR;
    }

    /**
     * @brief Reduced action space
     *
     * Compute the action space available at the given state.
     * @param {const std::vector<double &} s; state
     * @return Return the reduced action space.
     */
    std::vector<std::vector<double>> reduced_action_space(const state &s) {
        std::vector<std::vector<double>> ras;
        for(auto &a : action_space) {
            state s_p;
            if(envt->state_transition(s,a,s_p)) {
                ras.push_back(a);
            }
        }
        return ras;
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
        (void) s;//TODO: remove with other decision criteria
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
	std::vector<double> operator()(const state &s) {
        if(!pl.root_node.is_fully_expanded() || !decision_criterion(s)) {
            pl.build_uct_tree(s);
        }
        std::vector<double> ra = pl.get_recommended_action(pl.root_node);
        pl.root_node.move_to_child(pl.argmax_score(pl.root_node),s);
        return ra;
	}

    /**
     * @brief Process reward
     *
     * Process the resulting reward from transition (s,a,s_p)
     * @param {state &} s; state
     * @param {std::vector<double> &} a; action
     * @param {state &} s_p; next state
     */
    void process_reward(
        const state & s,
        const std::vector<double> & a,
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
