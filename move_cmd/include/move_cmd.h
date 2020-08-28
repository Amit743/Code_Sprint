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
    void surge(int tim);
    void sway(int tim, int vel);
    void heave(int tim, float dep);
    void yaw(double angle);

  private:
    ros::NodeHandle nh;
    ros::Publisher cmd_pub, mode_pub;

    enum Mode { SURFACE, HOVER, MOVEMENT };
    std_msgs::UInt8 mode;

    hammerhead_control::Position current_pos, final_pos, detect_pos;


};
#endif