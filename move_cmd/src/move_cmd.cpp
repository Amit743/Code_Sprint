#include "move_cmd/move_cmd.h"
move_cmd::move_cmd(ros::NodeHandle _nh): nh(_nh){
  cmd_pub = nh.advertise<hammerhead_control::MoveCmd>("/move_cmd", 100);
  mode_pub = nh.advertise<std_msgs::UInt8>("/set_mode", 10,true);
  bot_pos= nh.subscribe("bot_position",1,&move_cmd::distance,this);
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
void move_cmd::distance(const hammerhead_control::Position::ConstPtr &msg){
  pos.x=msg->x;
  pos.y=msg->y;
  pos.z=msg->z;
  pos.pitch=0;
  pos.roll=0;
  pos.yaw=msg->yaw;
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

void move_cmd::heaveD(float dep){
  if (dep==0)return;
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
  a.depth_speed = 2;
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
  angle=3.1415/180*angle;
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

  a.yaw_speed = 0.05;
  a.yaw_time = abs(angle)/a.yaw_speed;
  // a.yaw_time = (a.yaw_time>15)?15:a.yaw_time;
  a.yaw = angle;

  a.depth = 0;
  a.depth_speed = 0;
  a.depth_time = 0;

  a.isAbsolute = 0;
  a.mode_after_last_cmd = 1;
  a.wait_for_max_timer_to_timeout = 1;
  cmd_pub.publish(a);
  ros::spinOnce();
  ROS_INFO("test::yaw wait(%f)",a.yaw_time);
  ros::Duration(a.yaw_time).sleep();
  //enterHoverMode();
}
void move_cmd::revolve(double angle,double radius=1){
  enterMovementMode();
  int steps = (int)(abs(angle)/5);
  for(int i=0; i<steps;i++){
    surge(2*radius);
    yaw(sign(angle)*5);
    ROS_INFO("%f %c complete",100.0*i/steps,'%');
    ros::spinOnce();
  }
}
void move_cmd::surged(int d){
  ROS_INFO("test::surged(%d)",d);
  ROS_INFO("%f,%f",pos.x,pos.y);
  float x=pos.x;//+d*cos(pos.yaw*3.1415/180);
  float y=pos.y;//+d*sin(pos.yaw*3.1415/180);
  //ROS_INFO("%f,%f",x,y);
  while(sqrt(pow(pos.x-x,2)+pow(pos.y-y,2))<d){
      surge(sign(d)*2);
      ROS_INFO("%f,%f,%f",pos.x,pos.y,sqrt(pow(pos.x-x,2)+pow(pos.y-y,2)));
  }
}
void move_cmd::swayd(int d){
  ROS_INFO("test::swayd(%d)",d);
  ROS_INFO("%f,%f",pos.x,pos.y);
  float x=pos.x-d*sin(pos.yaw*3.1415/180);
  float y=pos.y+d*cos(pos.yaw*3.1415/180);
  while(sqrt(pow(pos.x-x,2)+pow(pos.y-y,2))>2){
      sway(sign(d)*2);
      ROS_INFO("%f,%f,%f",pos.x,pos.y,sqrt(pow(pos.x-x,2)+pow(pos.y-y,2)));
  }
}
void move_cmd::toPointR(int x,int y,int z){
  ROS_INFO("test::heave(%d)",z);
  ROS_INFO("%f,%f,%f",pos.x,pos.y,pos.z);
  heaveD(z);
  ROS_INFO("test::surge(%d)",x);
  ROS_INFO("%f,%f,%f",pos.x,pos.y,pos.z);
  ros::spinOnce();
  ROS_INFO("yaw %f",pos.yaw);
  yaw(-pos.yaw);
  ros::spinOnce();
  ROS_INFO("%f yaw",pos.yaw);
  float X=pos.x+x;
  while(sign(x)*(X-pos.x)>0){
      surge(sign(x)*2);
      ros::spinOnce();
      ROS_INFO("%f,%f",pos.x,pos.y);
  }
  ros::Duration(3).sleep();
  ROS_INFO("test::sway(%d)",y);
  ROS_INFO("%f,%f,%f",pos.x,pos.y,pos.z);
  ros::spinOnce();
  ROS_INFO("yaw %f",pos.yaw);
  yaw(sign(y)*90-pos.yaw);
  ros::spinOnce();
  ROS_INFO("yaw %f",pos.yaw);
  float Y=pos.y+y;
  while(sign(y)*(Y-pos.y)>0){
      surge(2);
      ros::spinOnce();
      ROS_INFO("%f,%f",pos.x,pos.y);
  }
  ros::spinOnce();
  ros::Duration(3).sleep();
  yaw(-pos.yaw);

}
void move_cmd::toPointDiagR(int x,int y,int z){
  float angle = 180/3.1415*atan2(y,x),dist=sqrt(x*x+y*y);
  ROS_INFO("%f,%f",angle,dist);

  ROS_INFO("test::heave(%d)",z);
  heaveD(z);
  ROS_INFO("test::yaw(%f)",angle);
  ros::spinOnce();
  yaw(angle-pos.yaw);
  ros::spinOnce();
  ROS_INFO("test::surge(%f)",dist);
  surged(dist);
  ros::spinOnce();
  ROS_INFO("test::yaw(%f)",-angle);
  yaw(-pos.yaw);
}
void move_cmd::toPointDiag(int x,int y,int z){
  ros::spinOnce();
  ROS_INFO("%f,%f,%f",pos.x,pos.y,pos.z);
  ROS_INFO("%f,%f,%f",x-pos.x,y-pos.y,z-pos.z);
  toPointDiagR(x-pos.x,y-pos.y,z-pos.z);
}
void move_cmd::toPoint(int x,int y,int z){
  ros::spinOnce();
  toPointR(x-pos.x,y-pos.y,z-pos.z);
}

void move_cmd::heaveAbs(int d){
  heaveD(d-pos.z);
}

int move_cmd::sign(int i){
  return (i<0)?-1:1;
}
int main(int argc, char **argv) {
  ros::init(argc, argv, "Move_Command_Generator");

  ros::NodeHandle nh;
  move_cmd auv(nh);
  ros::Rate loop_rate(50);
  //while (ros::ok()) {
    ros::spinOnce();
    ROS_INFO("test::enterHoverMode()");
    auv.enterHoverMode();
    ros::Duration(12).sleep();
    ROS_INFO("test::toPointDiagR()");
    auv.toPointDiagR(17,-8,130);
    ROS_INFO("test::revolve(-90,2)");
    auv.revolve(-90,2);
    ROS_INFO("test::toPointDiag()");
    auv.toPointDiag(-8,-128,1050);
    ROS_INFO("test::toPoint()");
    auv.toPoint(-8,-128,1050);
    //ros::Duration(10).sleep();
    //auv.toPointR(0,10,1);
    // auv.surged(10);
    // auv.swayd(-7);
    // auv.surged(5);
    //auv.revolve(180,2);
    // auv.sway(4)  ;
    // auv.surge(15);
    // auv.sway(-20);
    // auv.yaw(90);
    // auv.yaw(68);


    // while(auv.pos.x>4){
    //
    //   auv.surge(2);
    //   ROS_INFO("%f",auv.pos.x);
    // }
    //
    //
    //
    // // ROS_INFO("test::surge(-10)");
    // auv.surge(-10);
    // ROS_INFO("test::sway(10)");
    // auv.sway(10);
    // ROS_INFO("test::sway(-10)");
    // auv.sway(-10);
    // ROS_INFO("test::revolve(90)");
    // auv.revolve(360,2);
    // ROS_INFO("test::heave(1050)");
    // auv.heave(1000);
    // ROS_INFO("test::heave(980)");
    // auv.heave(980);

    // ROS_INFO("test::yaw(90)");
    // auv.yaw(90);
    // ROS_INFO("test::yaw(-90)");
    // auv.yaw(-180);
    // ROS_INFO("test::enterSurfaceMode()");
    //ros::Duration(5).sleep();
    auv.enterHoverMode();
    ros::Duration(5).sleep();
    auv.enterSurfaceMode();
    //
    //loop_rate.sleep();

  return 0;
}
