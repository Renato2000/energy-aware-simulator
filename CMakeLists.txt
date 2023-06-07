cmake_minimum_required(VERSION 3.2)
message(STATUS "Cmake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

project(energy-aware CXX)

add_definitions("-Wall -Wno-unused-variable -Wno-unused-private-field")

set(CMAKE_CXX_STANDARD 14)

# build the version number
set(ENERGY_AWARE_VERSION_MAJOR "1")
set(ENERGY_AWARE_VERSION_MINOR "0")
set(ENERGY_AWARE_VERSION_PATCH "0")
set(ENERGY_AWARE_VERSION_EXTRA "dev")

if (${ENERGY_AWARE_VERSION_PATCH} EQUAL "0")
    set(ENERGY_AWARE_RELEASE_VERSION "${ENERGY_AWARE_VERSION_MAJOR}.${ENERGY_AWARE_VERSION_MINOR}")
else ()
    set(ENERGY_AWARE_RELEASE_VERSION "$(ENERGY_AWARE_VERSION_MAJOR}.${ENERGY_AWARE_VERSION_MINOR}.${ENERGY_AWARE_VERSION_PATCH}")
endif ()

if (NOT ${ENERGY_AWARE_VERSION_EXTRA} EQUAL "")
    set(ENERGY_AWARE_RELEASE_VERSION "${ENERGY_AWARE_RELEASE_VERSION}-${ENERGY_AWARE_VERSION_EXTRA}")
endif ()

include_directories(src/ include/ /usr/local/include /usr/local/include/wrench)

# utils
set(UTILS
    src/utils/Task.h
    src/utils/Task.cpp
    src/utils/DAG.h
    src/utils/DAG.cpp
    src/utils/Executor.h
    src/utils/Executor.cpp
    src/utils/Executors.h
    src/utils/Executors.cpp
    src/utils/ClusterInfo.h
    src/utils/ClusterInfo.cpp
    src/utils/Hosts.h
    src/utils/Hosts.cpp
)

# scheduling algorithms
set(SCHEDULING_ALGORITHMS
    ${UTILS}
    src/scheduling_algorithm/EnRealAlgorithm.h
    src/scheduling_algorithm/EnRealAlgorithm.cpp
    src/scheduling_algorithm/IOAwareAlgorithm.h
    src/scheduling_algorithm/IOAwareAlgorithm.cpp
    src/scheduling_algorithm/IOAwareBalanceAlgorithm.h
    src/scheduling_algorithm/IOAwareBalanceAlgorithm.cpp
    src/scheduling_algorithm/SchedulingAlgorithm.h
    src/scheduling_algorithm/SPSSEBAlgorithm.h
    src/scheduling_algorithm/SPSSEBAlgorithm.cpp
    src/scheduling_algorithm/FifoAlgorithm.h
    src/scheduling_algorithm/FifoAlgorithm.cpp 
    src/scheduling_algorithm/EnergyAwareAlgorithm.h
    src/scheduling_algorithm/EnergyAwareAlgorithm.cpp
)

# source files
set(SOURCE_FILES
    ${UTILS}
    ${SCHEDULING_ALGORITHMS}    
    src/EnergyAwareSimulator.cpp
    src/EnergyAwareStandardJobScheduler.h
    src/EnergyAwareStandardJobScheduler.cpp
    src/GreedyWMS.h
    src/GreedyWMS.cpp
    src/PowerMeter.h
    src/PowerMeter.cpp
    src/EnergyMeter.h
    src/EnergyMeter.cpp
    src/cost_model/CostModel.h
    src/cost_model/TraditionalPowerModel.h
    src/cost_model/TraditionalPowerModel.cpp
)

set(TEST_FILES

)

# wrench libraries
find_library(WRENCH_LIBRARY NAMES wrench)
find_library(WRENCH_PEGASUS_LIBRARY NAMES wrenchpegasusworkflowparser)
find_library(SIMGRID_LIBRARY NAMES simgrid)
find_library(PUGIXML_LIBRARY NAMES pugixml)
find_library(GTEST_LIBRARY NAMES gtest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
add_executable(wrench-energy-aware ${SOURCE_FILES})
target_link_libraries(wrench-energy-aware ${WRENCH_LIBRARY} ${WRENCH_PEGASUS_LIBRARY} ${SIMGRID_LIBRARY} ${PUGIXML_LIBRARY})
install(TARGETS wrench-energy-aware DESTINATION bin)
