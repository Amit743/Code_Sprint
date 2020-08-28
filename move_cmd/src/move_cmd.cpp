#include "move_cmd.h"
move_cmd::move_cmd(ros::NodeHandle _nh): nh(_nh){
  cmd_pub = nh.advertise<hammerhead_control::MoveCmd>("/move_cmd", 100);
  mode_pub = nh.advertise<std_msgs::UInt8>("/set_mode", 10,true);
  ROS_INFO("move_cmd::move_cmd()");
}

move_cmd::~move_cmd(){

}

void move_cmd::enterHoverMode(){
  mode.data = HOVER;
  mode_pub.publish(mode);
  ros::spinOnce();

}

void move_cmd::enterMovementMode(){
  mode.data = MOVEMENT;
  mode_pub.publish(mode);
  ros::spinOnce();
}

void move_cmd::enterSurfaceMode(){
  mode.data = SURFACE;
  mode_pub.publish(mode);
  ros::spinOnce();

}

void move_cmd::surge(int tim){
  enterMovementMode();
  hammerhead_control::MoveCmd a;
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 1;
  a.is_depth = 0;
  a.is_sway = 0;
  a.is_yaw = 1;

  a.surge_speed = 5;
  a.surge_time = 10*tim;
  a.surge = 5;

  a.sway_speed = 0;
  a.sway_time = 0;
  a.sway = 0;

  a.yaw = a.yaw+0.01;
  a.yaw_speed = 5;
  a.yaw_time = tim;

  a.depth = 0;
  a.depth_speed = 0;
  a.depth_time = 0;

  a.isAbsolute = 0;
  a.mode_after_last_cmd = 1;
  a.wait_for_max_timer_to_timeout = 1;
  cmd_pub.publish(a);
  ros::spinOnce();
  ros::Duration(11*tim).sleep();
//  enterHoverMode();
}

void move_cmd::sway(int tim, int vel){
  enterMovementMode();
  hammerhead_control::MoveCmd a;
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 1;
  a.is_depth = 0;
  a.is_sway = 0;
  a.is_yaw = 0;

  a.surge_speed = 5;
  a.surge_time = tim;
  a.surge = 5;

  a.sway_speed = 0;
  a.sway_time = 0;
  a.sway = 0;

  a.yaw = 0;
  a.yaw_speed = 0;
  a.yaw_time = 0;

  a.depth = 0;
  a.depth_speed = 0;
  a.depth_time = 0;

  a.isAbsolute = 0;
  a.mode_after_last_cmd = 1;
  a.wait_for_max_timer_to_timeout = 1;
  cmd_pub.publish(a);
  ros::spinOnce();
  ros::Duration(tim).sleep();
  enterHoverMode();
}

void move_cmd::heave(int tim, float dep){
  enterMovementMode();
  hammerhead_control::MoveCmd a;
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 1;
  a.is_depth = 0;
  a.is_sway = 0;
  a.is_yaw = 0;

  a.surge_speed = 5;
  a.surge_time = tim;
  a.surge = 5;

  a.sway_speed = 0;
  a.sway_time = 0;
  a.sway = 0;

  a.yaw = 0;
  a.yaw_speed = 0;
  a.yaw_time = 0;

  a.depth = 0;
  a.depth_speed = 0;
  a.depth_time = 0;

  a.isAbsolute = 0;
  a.mode_after_last_cmd = 1;
  a.wait_for_max_timer_to_timeout = 1;
  cmd_pub.publish(a);
  ros::spinOnce();
  ros::Duration(tim).sleep();
  enterHoverMode();
}

void move_cmd::yaw(double angle){
  enterMovementMode();
  hammerhead_control::MoveCmd a;
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 1;
  a.is_depth = 0;
  a.is_sway = 0;
  a.is_yaw = 0;

  a.surge_speed = 5;
  //a.surge_time = tim;
  a.surge = 5;

  a.sway_speed = 0;
  a.sway_time = 0;
  a.sway = 0;

  a.yaw = 0;
  a.yaw_speed = 0;
  a.yaw_time = 0;

  a.depth = 0;
  a.depth_speed = 0;
  a.depth_time = 0;

  a.isAbsolute = 0;
  a.mode_after_last_cmd = 1;
  a.wait_for_max_timer_to_timeout = 1;
  cmd_pub.publish(a);
  ros::spinOnce();
  //ros::Duration(tim).sleep();
  enterHoverMode();
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "Move_Command_Generator");

  ros::NodeHandle nh;
  move_cmd auv(nh);
  ros::Rate loop_rate(30);
  while (ros::ok()) {
    ros::spinOnce();
    ROS_INFO("test::surge(10)");
    auv.surge(1);
    ROS_INFO("test::sleep(10)");
    //ros::Duration(3).sleep();
    //loop_rate.sleep();
 }
  return 0;
}
