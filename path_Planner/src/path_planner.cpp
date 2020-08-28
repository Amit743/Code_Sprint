#include "ros/ros.h"
#include "simulator_sauvc_test/Coordinates.h"
#include <cstdlib>
#include <iostream>
#include <std_msgs/Int8.h>

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

cv::Mat src;
simulator_sauvc_test::Coordinates object_server;
ros::ServiceClient object_client;

void initialHeave() {

  //MoveCommand *initial = new MoveCommand();
  //initial->heave();//TODO assign depth based on gate height

}

void preQualify_phase1() {

  bool complete = false;
  //MoveCommand *phase1_command = new MoveCommand(); 
 
  while(!complete) {
      object_server.request.dummy = 1;
      if(object_client.call(object_server)) {
       ROS_INFO("Gate centre Coordinates are: [%f,%f] ##",
              object_server.response.x[0],object_server.response.y[0]);
      }
      float Gate_center_x = object_server.response.x[0];
      float Gate_center_y = object_server.response.y[0];
      double distance;
      //distance =
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
  
}

void preQualify_phase2() {

  bool complete = false;
  //MoveCommand *phase2_command = new MoveCommand();
  while(!complete) {
      object_server.request.dummy = 1;
      if(object_client.call(object_server)) {
       ROS_INFO("Marker centre Coordinates are: [%f,%f] ##",
              object_server.response.x[0],object_server.response.y[0]);
      }
      float Marker_center_x = object_server.response.x[0];
      float Marker_center_y = object_server.response.y[0];
      double distance;
      //distance =
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

}

void preQualify_phase3() {
  
 // MoveCommand *phase3_command = new MoveCommand(); 
  
  object_server.request.dummy = 1;
  double distance;
  while(distance > 3) {
      
      if(object_client.call(object_server)) {
       ROS_INFO("Gate centre Coordinates are: [%f,%f] ##",
              object_server.response.x[0],object_server.response.y[0]);
      }
      float Gate_center_x = object_server.response.x[0];
      float Gate_center_y = object_server.response.y[0];
      //distance =
   //   phase3_command->surge(); // add value accordingly
  }

  //phase3_command->surge(8); //assume 8ft to surge for completion of final gate cross
  //delete phase3_command;
}

void finalSurfaceUP() {

  //MoveCommand *final_ = new MoveCommand();
  //final_->heave(); //set -ve of inital set in initalHeave();

}

int main(int argc, char **argv){
  
  ros::init(argc,argv,"PathPlanner");
  ros::NodeHandle nh;
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
