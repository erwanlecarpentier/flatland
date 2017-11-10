#ifndef OLUCT_POLICY_HPP_
#define OLUCT_POLICY_HPP_

#include <environment.hpp>
#include <uct.hpp>

/**
 * @brief OLUCT policy
 */
template <class DFTPLC>
class oluct {
public:
    unsigned decision_criterion_selector;
    environment * envt; ///< Pointer to an environment, used for action space reduction
    uct<DFTPLC> pl;
    double vmr_threshold;

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
        vmr_threshold = p.VMR_THRESHOLD;
    }

    /**
     * @brief Action validity test
     *
     * Test whether the recommended action is valid or not.
     * @param {const state &} s; the current state of the agent
     * @return Return true if the test does not discard the tree.
     */
    bool action_validity_test(const state &s) {
        return pl.model.is_action_valid(s,pl.get_recommended_action(pl.root_node));
    }

    /**
     * @brief State distribution VMR test
     *
     * Test whether the Variance-Mean-Ratio (VMR) of the state distribution at the node
     * reached by the recommended action is small enough.
     * Only the diagonal of the covariance matrix is tested.
     * The test is based on the VMR for the variances to be comparable.
     * @return Return true if the test does not discard the tree.
     */
    bool state_distribution_vmr_test() {
        node * v_ptr = pl.root_node.get_child_at(pl.argmax_score(pl.root_node));
        std::vector<state> samples = v_ptr->get_states();
        double one_over_n = 1. / ((double) samples.size());
        double vmr_x = 0.;
        double vmr_y = 0.;
        double vmr_v = 0.;
        double vmr_theta = 0.;
        double m_x = 0.;
        double m_y = 0.;
        double m_v = 0.;
        double m_theta = 0.;
        for(auto &s : samples) { // compute the means
            m_x += s.x;
            m_y += s.y;
            m_v += s.v;
            m_theta += s.theta;
        }
        m_x *= one_over_n;
        m_y *= one_over_n;
        m_v *= one_over_n;
        m_theta *= one_over_n;
        for(auto &s : samples) { // compute the vmr
            vmr_x += pow(s.x - m_x, 2.);
            vmr_y += pow(s.y - m_y, 2.);
            vmr_v += pow(s.v - m_v, 2.);
            vmr_theta += pow(s.theta - m_theta, 2.);
        }
        vmr_x *= one_over_n;
        vmr_y *= one_over_n;
        vmr_v *= one_over_n;
        vmr_theta *= one_over_n;
        double m_vmr = (vmr_x + vmr_y + vmr_v + vmr_theta) / 4.;
        return is_less_than(m_vmr,vmr_threshold);
    }

    /**
     * @brief Switch on decision criterion
     *
     * Select the decision criterion according to the chosen algorithm.
     * The tested tree is the current tree saved in the parameters.
     * @param {const state &} s; the current state of the agent
     * @return Return true if the sub-tree is kept.
     */
    bool decision_criterion(const state &s) {
        switch(decision_criterion_selector) {
            default: { // Plain decision criterion (no test)
                return true;
            }
            case 0: { // Plain with action validity test
                return action_validity_test(s);
            }
            case 1: { // State distribution variance test
                return state_distribution_vmr_test();
            }
        }
    }

    /**
     * @brief Remove waypoint
     *
     * Remove the waypoints of the environment model at the position of s.
     * OLUCT simply forwards this instruction to its underlying UCT policy.
     * @param {const state &} s; state
     * @return Return the number of reached waypoints.
     */
    unsigned remove_waypoints_at(const state &s) {
        return pl.remove_waypoints_at(s);
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
