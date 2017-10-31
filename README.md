# Flatland environment

The Flatland environment is a 2D environment provided for control algorithms
testing expressed in the Markov Decision Process classical formalism.
It features a discrete or continuous map generation with fully configurable
parameters.

# Use

Use the configuration files in the ./config/ repository in order to set the
simulation parameters.

In order to introduce a new policy, modify the ./src/policy/todo.hpp file,
indeed, the policy classes are template classes that supposes a few methods to
be implemented.

# Edit

30th of October 2017

