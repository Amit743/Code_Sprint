#include "move_cmd.h"
move_cmd::move_cmd(ros::NodeHandle _nh): nh(_nh){
  cmd_pub = nh.advertise<hammerhead_control::MoveCmd>("/move_cmd", 100);
  mode_pub = nh.advertise<std_msgs::UInt8>("/set_mode", 10,true);
  ROS_INFO("move_cmd::move_cmd()");
  a.wait_for_max_timer_to_timeout = 0;
  a.is_surge = 0;
  a.is_depth = 0;
  a.is_sway = 0;
  a.is_yaw = 0;

  a.surge_speed = 0;
  a.surge_time = 0;
  a.surge = 0;

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
  a.mode_after_last_cmd = 0;
  a.wait_for_max_timer_to_timeout = 0;
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
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 1;
  a.is_depth = 0;
  a.is_sway = 0;
  a.is_yaw = 0;

  a.surge_speed =sign(tim)*5;
  a.surge_time =abs(tim);
  a.surge = sign(tim)*5;

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
  ros::Duration(abs(tim)).sleep();
  //enterHoverMode();
}

void move_cmd::sway(int tim){
  enterMovementMode();
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 0;
  a.is_depth = 0;
  a.is_sway = 1;
  a.is_yaw = 0;

  a.surge_speed = 0;
  a.surge_time = 0;
  a.surge = 0;

  a.sway_speed =sign(tim)*5;
  a.sway_time = abs(tim);
  a.sway = sign(tim)*5;

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
  ros::Duration(abs(tim)).sleep();
  //enterHoverMode();
}

void move_cmd::heave(float dep){
  enterMovementMode();
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 0;
  a.is_depth = 1;
  a.is_sway = 0;
  a.is_yaw = 0;

  a.surge_speed = 0;
  a.surge_time = 0;
  a.surge = 0;

  a.sway_speed = 0;
  a.sway_time = 0;
  a.sway = 0;

  a.yaw = 0;
  a.yaw_speed = 0;
  a.yaw_time = 0;

  a.depth = dep;
  a.depth_speed = 5;
  a.depth_time = abs(dep)/a.depth_speed;

  a.isAbsolute = 0;
  a.mode_after_last_cmd = 1;
  a.wait_for_max_timer_to_timeout = 1;
  cmd_pub.publish(a);
  ros::spinOnce();
  ros::Duration(a.depth_time).sleep();

  //enterHoverMode();
}

void move_cmd::yaw(double angle){
  enterMovementMode();
  a.wait_for_max_timer_to_timeout = 1;
  a.is_surge = 0;
  a.is_depth = 0;
  a.is_sway = 0;
  a.is_yaw = 1;

  a.surge_speed = 0;
  a.surge_time = 0;
  a.surge = 0;

  a.sway_speed = 0;
  a.sway_time = 0;
  a.sway = 0;

  a.yaw_speed = 10;
  a.yaw_time = abs(angle)/a.yaw_speed;
  a.yaw = angle;

  a.depth = 0;
  a.depth_speed = 0;
  a.depth_time = 0;

  a.isAbsolute = 0;
  a.mode_after_last_cmd = 1;
  a.wait_for_max_timer_to_timeout = 1;
  cmd_pub.publish(a);
  ros::spinOnce();
  ros::Duration(a.yaw_time).sleep();
  //enterHoverMode();
}

int move_cmd::sign(int i){
  return (i<0)?-1:1;
}
int main(int argc, char **argv) {
  ros::init(argc, argv, "Move_Command_Generator");

  ros::NodeHandle nh;
  move_cmd auv(nh);
  ros::Rate loop_rate(30);
  //while (ros::ok()) {
    ros::spinOnce();
    ROS_INFO("test::enterHoverMode()");
    auv.enterHoverMode();
    ros::Duration(7).sleep();
    ROS_INFO("test::surge(10)");
    auv.surge(10);
    ROS_INFO("test::surge(-10)");
    auv.surge(-10);
    ROS_INFO("test::sway(10)");
    auv.sway(10);
    ROS_INFO("test::heave(1050)");
    auv.heave(1000);
    ROS_INFO("test::heave(980)");
    auv.heave(980);
    ROS_INFO("test::yaw(90)");
    auv.yaw(90);
    ROS_INFO("test::sway(-10)");
    auv.sway(-10);
    ROS_INFO("test::yaw(-90)");
    auv.yaw(-90);
    ROS_INFO("test::enterSurfaceMode()");
    ros::Duration(5).sleep();
    auv.enterHoverMode();
    ros::Duration(5).sleep();
    auv.enterSurfaceMode();
    //
    //loop_rate.sleep();

  return 0;
}
