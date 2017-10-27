#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

/**
 * @brief Random policy
 */
class random_policy {
public:
    /**
     * @brief Policy operator
     *
     * TODO
     */
	void operator()(std::vector<double> &s, std::vector<double> &a) {
        (void) s;
        a = std::vector<double> {1.,0.};
	}
};

#endif // RANDOM_POLICY_HPP_
