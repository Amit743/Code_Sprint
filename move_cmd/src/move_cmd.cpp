#include "move_cmd.h"
move_cmd::move_cmd(ros::NodeHandle _nh): nh(_nh){
  cmd_pub = nh.advertise<hammerhead_control::MoveCmd>("/move_cmd", 1);
  mode_pub = nh.advertise<std_msgs::UInt8>("/set_mode", 1);
  ROS_INFO("move_cmd::move_cmd()");
}

move_cmd::~move_cmd(){

}

void move_cmd::enterHoverMode(){
  mode.data = HOVER;
  mode_pub.publish(mode);
}

void move_cmd::enterMovementMode(){
  mode.data = MOVEMENT;
  mode_pub.publish(mode);
}

void move_cmd::enterSurfaceMode(){
  mode.data = SURFACE;
  mode_pub.publish(mode);
}

void move_cmd::surge(int tim){
  enterMovementMode();
  hammerhead_control::MoveCmd a;
  a.wait_for_max_timer_to_timeout = 1;
  a.is_depth = 1;
  a.depth_time = tim;
  a.depth_speed = 0.1;
  a.depth = 1200;

  a.is_yaw = 0;
  a.is_sway = 1;
  a.sway_speed = 5;
  a.sway_time = tim;
  a.is_surge = 0;
  cmd_pub.publish(a);
  ros::Duration(tim).sleep();
  //enterHoverMode();

}

void move_cmd::sway(int tim, int vel){

}

void move_cmd::heave(int tim, float dep){

}

void move_cmd::yaw(double angle){

}

int main(int argc, char **argv) {
  ros::init(argc, argv, "Move_Command_Generator");

  ros::NodeHandle nh;
  move_cmd auv(nh);
  ros::Rate loop_rate(30);
  while (ros::ok()) {
    ros::spinOnce();
    ROS_INFO("test::surge(10)");
    auv.surge(10);
    ROS_INFO("test::sleep(10)");
    ros::Duration(10).sleep();
    //loop_rate.sleep();
 }
  return 0;
}
