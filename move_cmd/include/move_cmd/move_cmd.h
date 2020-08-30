#ifndef MOVE_CMD_H
#define MOVE_CMD_H

#include <hammerhead_control/MoveCmd.h>
#include <hammerhead_control/Position.h>
#include <ros/package.h>
#include <ros/ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/UInt8.h>
#include <stdio.h>
class move_cmd {
  public:
    move_cmd(ros::NodeHandle);
    ~move_cmd();

    void enterHoverMode();
    void enterMovementMode();
    void enterSurfaceMode();
    void surge(int);
    void sway(int);
    void heaveD(float);
    void yaw(double);
    void revolve(double,double);
    void surged(int);
    void swayd(int);
    void heaveAbs(int);
    void toPointR(int,int,int);
    void toPoint(int,int,int);
    void toPointDiag(int,int,int);
    void toPointDiagR(int,int,int);
    hammerhead_control::Position pos;

  private:
    int sign(int);
    void distance(const hammerhead_control::Position::ConstPtr &);
    ros::NodeHandle nh;
    ros::Publisher cmd_pub, mode_pub;
    ros::Subscriber bot_pos;


    enum Mode { SURFACE, HOVER, MOVEMENT };
    std_msgs::UInt8 mode;
    hammerhead_control::MoveCmd a;
    hammerhead_control::Position current_pos, final_pos, detect_pos;


};
#endif
