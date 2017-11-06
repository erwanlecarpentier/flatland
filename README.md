# Flatland environment

The Flatland environment is a 2D environment provided for control algorithms
testing expressed in the Markov Decision Process classical formalism.
It features a discrete or continuous map generation with fully configurable
parameters.
The choice of policy and scenario is up to the users who can implement their
own classes.

# Use

Use the configuration files in the './config' repository in order to set the
simulation parameters.

In order to introduce a new policy, modify the './src/policy/todo.hpp' file.
The policy classes are template classes that suppose a few methods to be
implemented.

# Dependencies

The library makes use of standard c++ libraries. We also used several non
standard libraries, here is a complete list:
- 'libconfig.h++' available at https://github.com/hyperrealm/libconfig

Edit: 30th of October 2017

