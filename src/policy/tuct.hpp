#ifndef TUCT_HPP_
#define TUCT_HPP_

#include <environment.hpp>
#include <uct/uct.hpp>

/**
 * @brief TUCT policy
 */
template <class MD, class PL>
class tuct : public uct<MD,PL> {
public:
    /**
     * @brief Constructor
     *
     * Construct wrt the given parameters.
     * @param {const parameters &} p; parameters
     */
    tuct(const parameters &p) : uct<MD,PL>(p) {
        (void) p;
        /* TODO: set the parameters here */
    }

    /**
     * @brief Policy operator
     *
     * Policy operator for the undertaken action at given state.
     * @param {const state &} s; given state
     * @return Return the undertaken action at s.
     */
	std::shared_ptr<action> operator()(const state &s) {
        (void) s;
        //TODO
        return std::shared_ptr<action> (nullptr);//TRM
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
        return std::vector<double>{/* TODO: define the vales to backup here */};
    }
};

#endif // TUCT_HPP_
