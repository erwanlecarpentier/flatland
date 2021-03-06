#ifndef NODE_HPP_
#define NODE_HPP_

/**
 * @brief Node class
 *
 * General node class, for root and standard nodes.
 */
class node {
private :
    bool root; ///< True if the node is root i.e. labeled by a unique state instead of a family of states
    unsigned visits_count; ///< Number of visits during the tree expansion
    state s; ///<Unique labelling state for a root node
    std::shared_ptr<action> incoming_action; ///< Action of the parent node that led to this node
    std::vector<double> sampled_outcomes; ///< Sampled outcomes (returns)
    std::vector<state> sampled_states; ///< Sampled states for a standard node
    std::vector<std::shared_ptr<action>> local_action_space; ///< Available actions at this node (bandit arms)

public :
    node *parent; ///< Pointer to the parent node
    std::vector<node> children; ///< Vector of nodes children

    /**
     * @brief Empty constructor
     */
    node(){}

    /**
     * @brief Root node constructor
     *
     * Usually the first node to be created.
     * The provided action space is a vector containing all the actions and is
     * shuffled at the nodes creation.
     * @param {std::vector<std::shared_ptr<action>>} _local_action_space; copied then shuffled in
     * local action space of the node (bandit arms)
     */
    node(
        state _state,
        std::vector<std::shared_ptr<action>> _local_action_space) :
        s(_state)
    {
        root = true;
        local_action_space = _local_action_space;
        shuffle(local_action_space);
        visits_count = 0;
    }

    /**
     * @brief Non-root node constructor
     *
     * Used during the expansion of the tree.
     * @param {std::vector<std::shared_ptr<action>>} _local_action_space; copied then shuffled in
     * actions of the node (bandit arms)
     */
    node(
        node * _parent,
        std::shared_ptr<action> _incoming_action,
        state _new_state,
        std::vector<std::shared_ptr<action>> _local_action_space) :
        incoming_action(_incoming_action),
        parent(_parent)
    {
        root = false;
        visits_count = 0;
        sampled_states.push_back(_new_state);
        local_action_space = _local_action_space;
        shuffle(local_action_space);
    }

    /**
     * @brief Clear method
     *
     * Clear the sampled outcomes, the parent, the incoming action, the state/states,
     * the visit count and the children vector of the node.
     * Do not change the 'root' attribute, hence the status of the node.
     * Do not clear action space vector, hence the available actions still remain in the same
     * organisation order.
     */
    void clear_node() {
        parent = nullptr;
        visits_count = 0;
        s.set_to_default();
        //incoming_action->set_to_default();
        sampled_outcomes.clear();
        sampled_states.clear();
        children.clear();
    }

    /** @brief Set as root */
    void set_as_root() {
        root = true;
    }

    /** @brief Get the visits count of the node (non-root node) */
    unsigned get_visits_count() const {
        assert(!root);
        return visits_count;
    }

    /** @brief Get the number of children */
    unsigned get_nb_children() const {
        return children.size();
    }

    /** @brief Get a pointer to the last created child */
    node * get_last_child() {
        return &children.back();
    }

    /** @brief Get a pointer to a child given its indice */
    node * get_child_at(unsigned indice) {
        return &children.at(indice);
    }

    /** @brief Get the value of the node */
    double get_value() const {
        double value = 0.;
        for(auto &r : sampled_outcomes) {
            value += r;
        }
        return value / ((double) visits_count);
    }

    /** @brief Get the state of the node (root node) */
    const state & get_state() const {
        assert(root);
        return s;
    }

    /** @brief Get the number of sampled states (non-root node) */
    unsigned get_nb_sampled_states() const {
        assert(!root);
        return sampled_states.size();
    }

    /** @brief Get a copy of the sampled states of the node */
    std::vector<state> get_sampled_states() const {
        return sampled_states;
    }

    /** @brief Get a copy of the sampled outcomes of the node */
    std::vector<double> get_sampled_outcomes() const {
        return sampled_outcomes;
    }

    /** @brief Get a copy of the last sampled state among the states family (non-root node) */
    state get_last_sampled_state() const {
        assert(!root);
        return sampled_states.back();
    }

    /**
     * @brief Get state or last sampled state
     *
     * Get either the unique labelling state if node is root, or the last sampled state
     * if non-root.
     */
    state get_state_or_last() const {
        if(is_root()) {
            return get_state();
        } else {
            return get_last_sampled_state();
        }
    }

    /** @brief Get the incoming action of the node (non-root node) */
    std::shared_ptr<action> get_incoming_action() const {
        assert(!root);
        return incoming_action;
    }

    /** @brief Set the action space */
    void set_action_space(std::vector<std::shared_ptr<action>> as) {
        local_action_space = as;
    }

    /** @brief Shuffle the action space */
    void shuffle_action_space() {
        shuffle(local_action_space);
    }

    /** @brief Get a copy of the actions vector */
    std::vector<std::shared_ptr<action>> get_action_space() const {
        return local_action_space;
    }

    /** @brief Get one action of the node given its indice in the actions vector */
    std::shared_ptr<action> get_action_at(unsigned indice) const {
        return local_action_space.at(indice);
    }

    /** @brief Get the next expansion action among the available actions */
    std::shared_ptr<action> get_next_expansion_action() const {
        return local_action_space.at(children.size());
    }

    /** @brief Get the number of actions (arms of the bandit) */
    unsigned get_nb_of_actions() const {
        return local_action_space.size();
    }

    /** @brief Is fully expanded @return Return true if the node is fully expanded */
    bool is_fully_expanded() const {
        assert(get_nb_of_actions() != 0);
        return (get_nb_children() == get_nb_of_actions());
    }

    /** @brief Is root @return Return true if the node is a root node */
    bool is_root() const {
        return root;
    }

    /**
     * @brief Create a child
     *
     * Create a child based on the incoming action.
     * @param {std::shared_ptr<action> &} inc_ac; incoming action of the new child
     * @param {state &} state; first sampled state of the new child
     * @param {std::vector<std::shared_ptr<action>>} as; action space
     */
    void create_child(
        std::shared_ptr<action> &inc_ac,
        state &s,
        std::vector<std::shared_ptr<action>> as)
    {
        children.emplace_back(node(this,inc_ac,s,as));
    }

    /**
     * @brief Set state
     *
     * Copy the given state to the nodes state. Node should be root.
     * @param {state &} _s; copied state
     */
    void set_state(const state &_s) {
        assert(root);
        s = _s;
    }

    /**
     * @brief Add to state
     *
     * Add a new sampled state to the states. Node should not be root.
     * @param {state &} s; added state
     */
    void add_to_states(state &s) {
        assert(!root);
        sampled_states.push_back(s);
    }

    /**
     * @brief Visit count increment
     *
     * Increment the node's visits counter.
     */
    void increment_visits_count() {
        ++visits_count;
    }

    /**
     * @brief Add to value
     *
     * Add a sample to the sampled outcome.
     * Node should not be root.
     * @param {double} r; outcome sample value to be added
     */
    void add_to_value(double r) {
        assert(!root);
        sampled_outcomes.push_back(r);
    }

    /**
     * @brief Move to child
     *
     * The current root node takes the children, states and action vector of one of its
     * children and updates its state.
     * @param {unsigned} indice; indice of the moved child
     * @param {state &} new_state; new labelling state
     */
    void move_to_child(unsigned indice, const state &new_state) {
        assert(is_root());
        local_action_space = children[indice].get_action_space();
        sampled_states = children[indice].get_sampled_states();
        visits_count = children[indice].get_visits_count();
        sampled_outcomes = children[indice].get_sampled_outcomes();
        auto tmp = std::move(children[indice].children); // Temporary variable to prevent from overwriting
        for(auto &elt : tmp) {
            elt.parent = this;
        }
        children = std::move(tmp);
        s = new_state;
    }
};

#endif // NODE_HPP_
