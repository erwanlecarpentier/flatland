#ifndef OLUCT_POLICY_HPP_
#define OLUCT_POLICY_HPP_

#include <environment.hpp>
#include <uct.hpp>
#include <linear_algebra.hpp>

/**
 * @brief OLUCT policy
 */
template <class DFTPLC>
class oluct {
public:
    std::vector<bool> decision_criteria_selector; ///< Vector containing the boolean values of the activation of each decision criterion
    environment * envt; ///< Pointer to an environment, used for action space reduction
    uct<DFTPLC> pl; ///< Embedded UCT policy
    double vmr_threshold; ///< VMR threshold for the VMR test
    double distance_threshold;  ///< Threshold for the distance
    double outcome_variance_threshold;  ///< Threshold for the distance

    /**
     * @brief Constructor
     *
     * Constructu using the given parameters
     * @param {parameters &} p; parameters
     * @param {environment *} en; pointer to the environment, used for action space reduction
     */
    oluct(parameters &p, environment *en) :
        pl(p,en)
    {
        p.parse_decision_criterion(decision_criteria_selector);
        vmr_threshold = p.VMR_THRESHOLD;
        distance_threshold = p.DISTANCE_THRESHOLD;
        outcome_variance_threshold = p.OUTCOME_VARIANCE_THRESHOLD;
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
        std::vector<state> samples = pl.root_node.get_sampled_states();
        std::vector<Eigen::Vector4d> data;
        for(auto &smp : samples) {
            data.emplace_back(smp.x,smp.y,smp.v,smp.theta);
        }
        Eigen::Vector4d vmr = variance_mean_ratio(data);
        double m_vmr = (vmr(0) + vmr(1) + vmr(2) + vmr(3)) / 4.;
        return is_less_than(m_vmr,vmr_threshold);
    }

    /**
     * @brief Outcome distribution variance test
     *
     * TODO
     * @return Return true if the test does not discard the tree.
     */
    bool outcome_distribution_variance_test() {
        //TODO
    }

    /**
     * @brief Distance to state distribution test
     *
     * Compute the distances between the current state and every states of the sampled state
     * distribution.
     * Reject the tree if the minimum distance is greater than a selected threshold.
     * The used distance is the Mahalanobis distance (edit 10/11/2017).
     * @param {const state &} s; the current state of the agent
     * @return Return true if the sub-tree is kept.
     */
    bool distance_to_state_distribution_test(const state &s) {
        std::vector<state> samples = pl.root_node.get_sampled_states();
        Eigen::Vector4d s_vect(s.x,s.y,s.v,s.theta);
        std::vector<Eigen::Vector4d> data;
        for(auto &smp : samples) {
            data.emplace_back(smp.x,smp.y,smp.v,smp.theta);
        }
        return is_less_than(mahalanobis_distance(s_vect,data,1e-30),distance_threshold);
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
        bool keep_tree = true;
        if(decision_criteria_selector[1]) {
            keep_tree *= action_validity_test(s);
        }
        if(decision_criteria_selector[2]) {
            keep_tree *= state_distribution_vmr_test();
        }
        if(decision_criteria_selector[3]) {
            keep_tree *= distance_to_state_distribution_test(s);
        }
        if(decision_criteria_selector[4]) {
            keep_tree *= outcome_distribution_variance_test();
        }
        return keep_tree;
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
