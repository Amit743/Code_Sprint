cmake_minimum_required(VERSION 3.0.2)
project(move_cmd)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  roslib
  sensor_msgs
  std_msgs
  hammerhead
  hammerhead_control
  vectornav
#actionlib
  #actionlib_msgs
  genmsg
  message_generation
)

find_package(Boost REQUIRED COMPONENTS system)
generate_messages(
  DEPENDENCIES
  std_msgs
#  actionlib_msgs
)
#catkin_package(CATKIN_DEPENDS message_runtime std_msgs)


include_directories(
 include
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

add_library(move_cmd_lib include/move_cmd/move_cmd.h src/move_cmd.cpp)
target_link_libraries(move_cmd_lib ${catkin_LIBRARIES})

add_executable(move_cmd_test src/move_cmd.cpp)
target_link_libraries(move_cmd_test move_cmd_lib ${catkin_LIBRARIES})
add_dependencies(move_cmd_test ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

catkin_package(CATKIN_DEPENDS message_runtime std_msgs
	INCLUDE_DIRS include
	LIBRARIES move_cmd_lib
)
