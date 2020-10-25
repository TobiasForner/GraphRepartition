# Graph Decomposition Tree based on Connectivity
Code for dynamically maintaining a connectivity-based graph decomposition tree.

# Usage
- mkdir mybuild
- cd mybuild
- cmake ..
- make
- cd src
- usage: ./cluster-alg <path-to-config>
- example configs can be found in the configs directory

# Options
- BUILD_TESTS: set to ON for tests to be build (cmake option -DBUILD_TESTS=ON), defaults to OFF. Requires catch.h to be inserted into the tests directory
- WITH_DEBUG: set to ON in order to enable debug checks (cmake option -DWITH_DEBUG=ON), defaults to OFF, this drastically increases running times
- GATHER_STATS: set to ON in order to enable the collection of stats and periodic printing of stats (cmake option -DGATHER_STATS=ON); also requires USE_LOGGING to be enabled
- USE_LOGGING: set to ON to enable more logging. this requires spdlog in the src directory; default is OFF
- USE_METIS: sets whether METIS and ParMETIS should be used. This requires ParMETIS to be in the lib directory

# Tests
- requires catch.h to be inserted into the tests directory
- call cmake with -DBUILD_TESTS=ON
- ./decomposition-test
