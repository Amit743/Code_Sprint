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

using namespace std;
using namespace cv;
#define K 200
//cm
#define gateHeight 100
#define gateWidth 200
#define pixelWidth 648
#define pixelHeight 488
#define fx 200
#define fy 200
#define to_t_surge 5
#define to_t_sway 2
 
ros::Publisher enable_yellow_flare_server_pub=nh.advertise<std_msgs::UInt8>("/enable_yellow_flare_server",1);
ros::Publisher enable_gate_server_pub=nh.advertise<std_msgs::UInt8>("/enable_gate_server",1);
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

void initialHeave() {

  //MoveCommand *initial = new MoveCommand();
  //initial->heave();//TODO assign depth based on gate height

}

void preQualify_phase1() {    // gate detection

  bool complete = false;
  //move_cmd *phase1_command = new MoveCommand();
  double Gate_center_x, Gate_center_y, width, height;
  while(!complete) {
      enabler.data = 1;
      enable_gate_server_pub.publish(enabler);
      object_server.request.dummy = 1;
      if(object_client.call(object_server)) {
       ROS_INFO("Gate centre Coordinates are: [%f,%f] ##",
              object_server.response.x[0],object_server.response.y[0]);
       Gate_center_x = object_server.response.x[0];
       Gate_center_y = object_server.response.y[0];
       width = object_server.response.w[0];
       height = object_server.response.h[0];
      }

      double distance;
      //distance = Distance(Gate_center_x, Gate_center_y, width, height);
      if(distance < 3) {

          object_server.request.dummy = 0;
         // phase1_command->surge(28);//in ft.s value assigned can change ur wish

      }
      else {

          //phase1_command->surge();//TODO assign value here accordingly
      }
      //add an logic at end to come out of the while loop like if complete crossing the gate, set complete = true
  }
  //delete phase1_command;

  // stop server
  enabler.data = 0;
  enable_gate_server_pub.publish(enabler);

}

void preQualify_phase2() {    // yellow flare

  bool complete = false;
  //MoveCommand *phase2_command = new MoveCommand();
  double Marker_center_x, Marker_center_y, width, height;
  while(!complete) {
      enabler.data = 1;
      enable_yellow_flare_server_pub.publish(enabler);
      object_server.request.dummy = 1;
      if(object_client.call(object_server)) {
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

void preQualify_phase3() {

 // MoveCommand *phase3_command = new MoveCommand();

  object_server.request.dummy = 1;
  double distance;
  double Gate_center_x, Gate_center_y, width, height;
  while(distance > 3) {
    enabler.data = 1;
    enable_gate_server_pub.publish(enabler);

      if(object_client.call(object_server)) {
       ROS_INFO("Gate centre Coordinates are: [%f,%f] ##",
              object_server.response.x[0],object_server.response.y[0]);
       Gate_center_x = object_server.response.x[0];
       Gate_center_y = object_server.response.y[0];
       width = object_server.response.w[0];
       height = object_server.response.h[0];
      }

      //distance = Distance(Gate_center_x, Gate_center_y, width, height);
   //   phase3_command->surge(); // add value accordingly
  }

  //phase3_command->surge(8); //assume 8ft to surge for completion of final gate cross
  //delete phase3_command;

  // stop server
  enabler.data = 0;
  enable_gate_server_pub.publish(enabler);

}

void finalSurfaceUP() {

  //MoveCommand *final_ = new MoveCommand();
  //final_->heave(); //set -ve of inital set in initalHeave();

}

int main(int argc, char **argv){

  ros::init(argc,argv,"PathPlanner");
  // ros::NodeHandle nh;
  ROS_INFO("Prequalification starts");


  //gate_client = nh.serviceClient<simulator_sauvc_test::Coordinates>("gate_coordinates");
  initialHeave();
  preQualify_phase1();
  preQualify_phase2();
  preQualify_phase3();
  finalSurfaceUP();
  ROS_INFO("Prequalification overs");

  ros::spin();
  return 0;
}
