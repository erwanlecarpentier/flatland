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
    double sdm_ratio; ///< SDM ratio (State Modality test)
    double sdv_threshold; ///< VMR threshold for the VMR test
    double sdsd_threshold;  ///< Threshold for the distance
    double rdv_threshold;  ///< Threshold for the distance

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
        p.parse_decision_criterion(decision_criteria_selector);
        sdm_ratio = p.SDM_RATIO;
        sdv_threshold = p.SDV_THRESHOLD;
        sdsd_threshold = p.SDSD_THRESHOLD;
        rdv_threshold = p.RDV_THRESHOLD;
    }

    /**
     * @brief Action validity test
     *
     * Test whether the recommended action is valid or not.
     * @param {const state &} s; the current state of the agent
     * @return Return true if the test does not discard the tree.
     */
    bool action_validity_test(const state &s) {
        unsigned indice = 0;
        return pl.model.is_action_valid(s,pl.get_recommended_action(pl.root_node,indice));
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
        return is_less_than(m_vmr,sdv_threshold);
    }

    /**
     * @brief Distance to state distribution test
     *
     * Compute the distances between the current state and the mean of the sampled state
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
        return is_less_than(mahalanobis_distance(s_vect,data,1e-30),sdsd_threshold);
    }

    /**
     * @brief Outcome distribution variance test
     *
     * Test whether the variance of the outcome distribution at the node reached by the
     * recommended action is small enough.
     * @return Return true if the test does not discard the tree.
     */
    bool outcome_distribution_variance_test() {
        std::vector<double> outcomes = pl.root_node.get_sampled_outcomes();
        double variance = 0.;
        for(unsigned i=0; i<outcomes.size(); ++i) {
            for(unsigned j=0; j<i; ++j) {
                variance += pow(outcomes[i] = outcomes[j], 2.);
            }
        }
        variance /= pow(((double) outcomes.size()),2.);
        return is_less_than(variance,rdv_threshold);
    }

    bool are_states_equal(const state &a, const state &b) const {
        if(is_equal_to(a.x,b.x)
        && is_equal_to(a.y,b.y)
        && is_equal_to(a.v,b.v)
        && is_equal_to(a.theta,b.theta)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief State multimodality test decision criterion
     *
     * Keep the sub-tree if there is only one state mode, up to a certain precision.
     * Also discard the tree if the current state does not lie in this mode.
     * The test is implemented for the discrete case.
     * @param {const state &} s; current state of the agent
     * @return Return true if the test does not discard the tree.
     */
    bool state_multimodality_test(const state &s) {
        std::vector<state> modes_values;
        std::vector<unsigned> modes_counters;
        for(auto &si : pl.root_node.get_sampled_states()) {
            bool is_new_mode = true;
            for(auto &m : modes_values) {
                if(are_states_equal(si,m)) {
                    is_new_mode = false;
                }
            }
            if(is_new_mode) {
                modes_values.push_back(si);
                modes_counters.push_back(1);
            } else {
                for(unsigned j=0; j<modes_values.size(); ++j) {
                    if(are_states_equal(si,modes_values[j])) {
                        modes_counters.at(j)++;
                    }
                }
            }
        }
        if(modes_values.size() == 1) { // mono-modal
            if(are_states_equal(s,modes_values[0])) { // state within the unique mode
                return true;
            } else { // state out of the unique mode
                return false;
            }
        } else { // multi-modal
            std::vector<double> modes_ratio;
            for(auto &elt: modes_counters) { // build modes ratio vector
                modes_ratio.push_back(((double) elt) / ((double)pl.root_node.get_sampled_states().size()));
            }
            unsigned state_mode_indice = 0;
            for(unsigned j=0; j<modes_values.size(); ++j) {
                if(are_states_equal(s,modes_values[j])) {
                    state_mode_indice = j;
                    break;
                }
            }
            if(is_less_than(modes_ratio.at(state_mode_indice),sdm_ratio)) {
                return false;
            } else {
                return true;
            }
        }
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
            keep_tree *= state_multimodality_test(s);
        }
        if(decision_criteria_selector[3]) {
            keep_tree *= state_distribution_vmr_test();
        }
        if(decision_criteria_selector[4]) {
            keep_tree *= distance_to_state_distribution_test(s);
        }
        if(decision_criteria_selector[5]) {
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
        unsigned indice = 0;
        std::shared_ptr<action> ra = pl.get_recommended_action(pl.root_node,indice);
        pl.root_node.move_to_child(indice,s);
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
