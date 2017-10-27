#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

/**
 * @brief Random policy
 */
struct random_policy {
    /**
     * @brief Policy operator
     *
     *
     */
	void operator()(std::vector<double> &s, std::vector<double> &a) {
        (void) s;
        a = std::vector<double> {1.,0.};
	}
};

#endif // RANDOM_POLICY_HPP_
