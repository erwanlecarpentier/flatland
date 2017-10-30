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
    double value; ///< Value function estimate
    unsigned visits_count; ///< Number of visits during the tree expansion
    std::vector<double> state; ///<Unique labelling state for a root node
    std::vector<double> incoming_action; ///< Action of the parent node that led to this node
    std::vector<std::vector<double>> states; ///< Sampled states for a standard node
    std::vector<std::vector<double>> actions; ///< Possible actions at this node (bandit arms)

public :
    node *parent; ///< Pointer to the parent node
    std::vector<node> children; ///< Vector of nodes children

    /**
     * @brief Root node constructor
     *
     * Usually the first node to be created. The provided action space is a vector containing
     * all the actions and is shuffled at the nodes creation.
     * @param {std::vector<std::vector<double>>} action_space; copied then shuffled in
     * actions of the node (bandit arms)
     */
    node(
        std::vector<double> _state,
        std::vector<std::vector<double>> action_space) :
        state(_state)
    {
        root = true;
        actions = action_space;
        shuffle(actions);
        visits_count = 0;
    }

    /**
     * @brief Standard node constructor
     *
     * Used during the expansion of the tree.
     * @param {std::vector<std::vector<double>>} action_space; copied then shuffled in
     * actions of the node (bandit arms)
     */
    node(
        node * _parent,
        std::vector<double> _incoming_action,
        std::vector<double> _new_state,
        std::vector<std::vector<double>> action_space) :
        incoming_action(_incoming_action),
        parent(_parent)
    {
        root = false;
        value = 0.;
        visits_count = 0;
        states.push_back(_new_state);
        actions = action_space;
        shuffle(actions);
    }

    /**
     * @brief Clear method
     *
     * Clear the value; the parent; the incoming action; the state/states; the visit count
     * and the children vector of the node. Do not change the value of 'root' attribute,
     * hence the status of the node. Do not clear actions vector, hence the available actions
     * still remain in the same organisation order.
     */
    void clear_node() {
        parent = nullptr;
        value = 0;
        visits_count = 0;
        state.clear();
        incoming_action.clear();
        states.clear();
        children.clear();
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
        return value;
    }

    /** @brief Get the state of the node (root node) */
    std::vector<double> get_state() const {
        assert(root);
        return state;
    }

    /** @brief Get the number of sampled states (non-root node) */
    unsigned get_nb_sampled_states() const {
        assert(!root);
        return states.size();
    }

    /** @brief Get a copy of the states vector of the node */
    std::vector<std::vector<double>> get_states() const {
        return states;
    }

    /** @brief Get a copy of the last sampled state among the states family (non-root node) */
    std::vector<double> get_last_sampled_state() const {
        assert(!root);
        return states.back();
    }

    /** @brief Get the incoming action of the node (non-root node) */
    std::vector<double> get_incoming_action() const {
        assert(!root);
        return incoming_action;
    }

    /** @brief Get the visits count of the node (non-root node) */
    unsigned get_visits_count() const {
        assert(!root);
        return visits_count;
    }

    /** @brief Get a copy of the actions vector */
    std::vector<std::vector<double>> get_actions() const {
        return actions;
    }

    /** @brief Get one action of the node given its indice in the actions vector */
    std::vector<double> get_action_at(unsigned indice) const {return actions.at(indice);}

    /** @brief Get the next expansion action among the available actions */
    std::vector<double> get_next_expansion_action() const {
        return actions.at(children.size());
    }

    /** @brief Get the number of actions (arms of the bandit) */
    unsigned get_nb_of_actions() const {
        return actions.size();
    }

    /** @brief Is fully expanded @return Return true if the node is fully expanded */
    bool is_fully_expanded() const {
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
     * @param {std::vector<double> &} inc_ac; incoming action of the new child
     * @param {std::vector<double> &} new_state; first sampled state of the new child
     */
    void create_child(std::vector<double> &inc_ac, std::vector<double> &new_state) {
        children.emplace_back(node(this,inc_ac,new_state,actions));
    }

    /**
     * @brief Set state
     *
     * Copy the given state to the nodes state. Node should be root.
     * @param {std::vector<double> &} s; copied state
     */
    void set_state(std::vector<double> &s) {
        assert(root);
        state = s;
    }

    /**
     * @brief Add to state
     *
     * Add a new sampled state to the states. Node should not be root.
     * @param {std::vector<double> &} s; added state
     */
    void add_to_states(std::vector<double> &s) {
        assert(!root);
        states.push_back(s);
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
     * Add a value to the nodes value. Node should not be root.
     * @param {double} r; value to be added
     */
    void add_to_value(double r) {
        assert(!root);
        value += r;
    }

    /**
     * @brief Move to child
     *
     * The current root node takes the children, states and action vector of one of its
     * children and updates its state.
     * @param {unsigned} indice; indice of the moved child
     * @param {std::vector<double> &} new_state; new labelling state
     */
    void move_to_child(unsigned indice, std::vector<double> &new_state) {
        assert(is_root());
        actions = children[indice].get_actions();
        states = children[indice].get_states();
        auto tmp = std::move(children[indice].children); // Temporary variable to prevent from overwriting
        for(auto &elt : tmp) {
            elt.parent = this;
        }
        children = std::move(tmp);
        state = new_state;
    }
};

#endif // NODE_HPP_
