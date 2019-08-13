cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED COMPONENTS filesystem system program_options)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS}/boost)
link_directories(${Boost_LIBRARY_DIRS})	

find_package(ICU REQUIRED COMPONENTS i18n uc)
include_directories(${ICU_INCLUDE_DIRS})
link_directories(${ICU_LIBRARY_DIRS})

find_package(PkgConfig REQUIRED)

pkg_check_modules(Eigen3 eigen3 REQUIRED)
include_directories(${Eigen3_INCLUDE_DIRS})

add_executable(smoothvmd smoothvmd_main.cc smooth_reduce.cc smoothvmd.cc reducevmd.cc MMDFileIOUtil.cc VMD.cc interpolate.cc fpschanger.cc)
target_link_libraries(smoothvmd ${Boost_LIBRARIES})
target_link_libraries(smoothvmd ${ICU_LIBRARIES})

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

enable_testing()
add_subdirectory(UnitTest)
