#include "ros/ros.h"
#include "simulator_sauvc_test/Coordinates.h"
#include <cstdlib>
#include <iostream>
#include <std_msgs/Int8.h>
#include <std_msgs/UInt8.h>

#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>
#include <sensor_msgs/Image.h>

#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include "std_msgs/Int32.h"
#include "std_msgs/String.h"

#include <simulator_sauvc_test/Coordinates.h>
#include <move_cmd/move_cmd.h>

using namespace std;
using namespace cv;
#define K 200
//cm
#define gateHeight 1.524
#define gateWidth 3.048
#define pixelWidth 648
#define pixelHeight 488
#define fx 50
#define fy 50

std_msgs::UInt8 enabler;

ros::Publisher enable_yellow_flare_server_pub;
ros::Publisher enable_gate_server_pub;
simulator_sauvc_test::Coordinates object_server;
ros::ServiceClient object_client1,object_client2;
move_cmd * auv;


struct target{
  float x,y,d;
};

target Distance(double center_x, double center_y, double width, double height) {
  target p;
  p.d=0.9*fy*gateHeight/height+0.1*fx*gateWidth/width;
  p.x=center_x/fx*p.d;
  p.y=center_y/fy*p.d;
  return p;
}

void start() {
  ROS_INFO("hover mode initiated");
  auv->enterHoverMode();
  ros::Duration(10).sleep();
  ROS_INFO("surface depth reached");

}

void gate() {    // gate detection
  int x=17,y=-9,z=100;
  auv->toPoint(x,y,z);
  // auv->surged(x);
  // auv->heave(z);
  // auv->swayd(y);
  // auv->revolve(-90,2);
  // auv->sway(10);
}

void marker1() {    // yellow flare

  bool complete = false;
  //MoveCommand *phase2_command = new MoveCommand();
  double Marker_center_x, Marker_center_y, width, height;
  while(!complete) {
      enabler.data = 1;
      enable_yellow_flare_server_pub.publish(enabler);
      object_server.request.dummy = 1;
      if(object_client2.call(object_server)) {
       ROS_INFO("Marker centre Coordinates are: [%f,%f] ##",
              object_server.response.x[0],object_server.response.y[0]);
       Marker_center_x = object_server.response.x[0];
       Marker_center_y = object_server.response.y[0];
       width = object_server.response.w[0];
       height = object_server.response.h[0];
      }
      double distance;
      //distance = Distance(Gate_center_x, Gate_center_y, width, height);
      if(distance < 3) {

         /* object_server.request.dummy = 0;
          phase2_command->sway(-5); // -ve for left sway
          phase2_command->surge();  //add surge values accordingly
          phase2_command->yaw(90);
          phase2_command->surge();
          phase2_command->yaw(90);
          phase2_command->surge();
          phase2_command->sway(5);
*/
      }
      else {

         // phase2_command->surge();//TODO assign value here accordingly
      }
      //add an logic at end to come out of the while loop like if complete moving around the marker, set complete = true
  }
  //delete phase2_command;

  // stop server
  enabler.data = 0;
  enable_yellow_flare_server_pub.publish(enabler);

}

void hexagon() {

  int x=0,y=-130;

  //auv->yaw()
  auv->surged(x);
  auv->swayd(y);

}

void stop() {
  ROS_INFO("hover mode initiated");
  auv->enterHoverMode();
  ROS_INFO("surface mode initiated");
  auv->enterSurfaceMode();
}

int main(int argc, char **argv){

  ros::init(argc,argv,"PathPlanner");
  ros::NodeHandle nh;
  enable_yellow_flare_server_pub=nh.advertise<std_msgs::UInt8>("/enable_yellow_flare_server",1);
  enable_gate_server_pub=nh.advertise<std_msgs::UInt8>("/enable_gate_server",1);

  object_client1=nh.serviceClient<simulator_sauvc_test::Coordinates>("gate_coordinates");
  object_client2=nh.serviceClient<simulator_sauvc_test::Coordinates>("yellow_flare_coordinates");
  auv=new move_cmd(nh);
  ROS_INFO("Prequalification starts");

  start();
  gate();
  //marker1();
  hexagon();
  stop();
  ROS_INFO("Prequalification overs");

  ros::spin();
  return 0;
}
