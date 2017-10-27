#ifndef UTILS_HPP_
#define UTILS_HPP_

constexpr double COMPARISON_THRESHOLD = 1e-6;

/**
 * @brief Print
 *
 * Generic printing method. Template method.
 */
template <class T>
void print(T t) {
	std::cout << t << std::endl;
}

/**
 * @brief Print
 *
 * Generic printing method for containers. Template method.
 */
template <class T>
void printv(std::vector<T> v) {
	for (auto &elt : v) {
		std::cout << elt << " ";
	}
	std::cout << std::endl;
}

/**
 * @brief Equality comparison
 *
 * Template method.
 * @return Return true if a < b up to a certain precision defined with COMPARISON_THRESHOLD
 * variable.
 */
template <class T1, class T2>
constexpr bool is_equal_to(T1 a, T2 b) {
    return std::fabs(a-b)<COMPARISON_THRESHOLD;
}

/**
 * @brief Strict inferiority comparison
 *
 * Template method.
 * @return Return true if a < b up to a certain precision defined with COMPARISON_THRESHOLD
 * variable.
 */
template <class T1, class T2>
constexpr bool is_less_than(T1 a, T2 b) {
    return a<(b-COMPARISON_THRESHOLD);
}

/**
 * @brief Strict superiority comparison
 *
 * Template method.
 * @return Return true if a < b up to a certain precision defined with COMPARISON_THRESHOLD
 * variable.
 */
template <class T1, class T2>
constexpr bool is_greater_than(T1 a, T2 b) {
    return a>(b+COMPARISON_THRESHOLD);
}

/**
 * @brief Sign function
 *
 * Template method.
 * @return Return -1. if x < 0., +1. else
 */
template <class T>
double sign(T x) {
    return (is_less_than(x,0.)) ? -1. : 1.;
}

/**
 * @brief Shuffle
 *
 * Shuffle the content of the vector randomly. Template method.
 */
template <class T>
inline void shuffle(std::vector<T> &v) {
    std::random_shuffle(v.begin(), v.end());
}

/**
 * @brief Random indice
 *
 * Pick a random indice of the input vector. You should initialize a random seed when
 * executing your program. Template method.
 * @param {const std::vector<T> &} v; input vector
 * @return Return a random indice.
 */
template <class T>
inline unsigned rand_indice(const std::vector<T> &v) {
    assert(v.size() != 0);
    return rand() % v.size();
}

/**
 * @brief Random element
 *
 * Pick a random element of the input vector. You should initialize a random seed when
 * executing your program. Template method.
 * @param {const std::vector<T> &} v; input vector
 * @return Return a random element.
 */
template <class T>
inline T rand_element(const std::vector<T> &v) {
    return v.at(rand_indice(v));
}

/**
 * @brief Argmax
 *
 * Get the indice of the maximum element in the input vector, ties are broken arbitrarily.
 * Template method.
 * @param {const std::vector<T> &} v; input vector
 * @return Return the indice of the maximum element in the input vector.
 */
template <class T>
inline unsigned argmax(const std::vector<T> &v) {
    auto maxval = *std::max_element(v.begin(),v.end());
    std::vector<unsigned> up_ind;
    for (unsigned j=0; j<v.size(); ++j) {
        if(!is_less_than(v[j],maxval)) {up_ind.push_back(j);}
    }
    return rand_element(up_ind);
}

/**
 * @brief Argmin
 *
 * See 'argmax' method. Template method.
 */
template <class T>
inline unsigned argmin(const std::vector<T> &v) {
    auto minval = *std::min_element(v.begin(),v.end());
    std::vector<unsigned> lo_ind;
    for (unsigned j=0; j<v.size(); ++j) {
        if(!is_greater_than(v[j],minval)) {lo_ind.push_back(j);}
    }
    return rand_element(lo_ind);
}

/**
 * @brief Uniformly distributed integer
 *
 * Generate a uniformly distributed integer
 * @return Return the sample
 */
int uniform_integer(int int_min, int int_max) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(int_min,int_max);
    return distribution(generator);
}

/**
 * @brief Uniformly distributed double
 *
 * Generate a uniformly distributed double
 * @return Return the sample
 */
double uniform_double(double double_min, double double_max) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(double_min,double_max);
    return distribution(generator);
}

/**
 * @brief Normally distributed double
 *
 * Generate a normally distributed double
 * @return Return the sample
 */
double normal_double(double mean, double stddev) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::normal_distribution<double> distribution(mean,stddev);
    return distribution(generator);
}

#endif // UTILS_HPP_
