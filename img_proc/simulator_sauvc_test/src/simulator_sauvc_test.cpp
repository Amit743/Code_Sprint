#include "simulator_sauvc_test/simulator_sauvc_test.h"
using namespace std;
using namespace cv;

SimSauvcTest::SimSauvcTest(ros::NodeHandle _nh) : nh(_nh), it(_nh) {
  enable_ip_front_sub = nh.subscribe("/enable_ip_front_cam", 1, &SimSauvcTest::enableFront, this);
  enable_ip_bottom_sub = nh.subscribe("/enable_ip_bottom_cam", 1, &SimSauvcTest::enableBottom, this);

  enable_gate_server_sub = nh.subscribe("/enable_gate_server", 1, &SimSauvcTest::enableGateServer, this);
  enable_bucket_server_sub = nh.subscribe("/enable_bucket_server", 1, &SimSauvcTest::enableBucketServer, this);
  enable_flare_server_sub = nh.subscribe("/enable_flare_server", 1, &SimSauvcTest::enableFlareServer, this);
  enable_yellow_flare_server_sub = nh.subscribe("/enable_yellow_flare_server", 1, &SimSauvcTest::enableYellowFlareServer, this);

  yellow_flare_detect_status_pub = nh.advertise<std_msgs::UInt8>("/yellow_flare_detection_status", 1, true);
  gate_detect_status_pub = nh.advertise<std_msgs::UInt8>("/gate_detection_status", 1, true);
  bucket_detect_status_pub = nh.advertise<std_msgs::UInt8>("/bucket_detection_status", 1, true);
  red_flare_detect_status_pub = nh.advertise<std_msgs::UInt8>("/red_flare_detection_status", 1, true);

  YF=ImageProcessor(ros::package::getPath("simulator_sauvc_test")+"/config/yellow_flare_ip.conf");
  RF=ImageProcessor(ros::package::getPath("simulator_sauvc_test")+"/config/red_flare_ip.conf");
  BU=ImageProcessor(ros::package::getPath("simulator_sauvc_test")+"/config/bucket_ip.conf");
  GT=ImageProcessor(ros::package::getPath("simulator_sauvc_test")+"/config/gate_ip.conf");
  GT_green=ImageProcessor(ros::package::getPath("simulator_sauvc_test")+"/config/gate_green_ip.conf");

  has_image_received = false;
  enabled.data = 1; disabled.data = 0;
  status_gate = status_bucket = status_flare = status_yellow_flare = 0;
  detection_status_gate = detection_status_bucket = detection_status_red_flare = detection_status_yellow_flare = 0;
}

SimSauvcTest::~SimSauvcTest() {}

void SimSauvcTest::enableFront(const std_msgs::UInt8::ConstPtr &msg) {

  if (isEnabledFront() == (msg->data))
    return;

  if (msg->data) {
    ip_status_front = 1;
    front_camera_sub =
        it.subscribe("/front_camera/image_rect_color", 1,
                     &SimSauvcTest::process_next_image_front, this);
    std::cout << "\n front camera subscribe \n" << '\n';

  } else {
    cout<<"\n shutdown\n";
    ip_status_front = 0;
    front_camera_sub.shutdown();
  }
}

void SimSauvcTest::process_next_image_front(
    const sensor_msgs::ImageConstPtr &image_frame) {

      // cout<<"\n inside process frame front\n";

  try {
    cv_bridge::toCvShare(image_frame, "bgr8")->image.copyTo(front_image);
    // cout<<" tried\n";
  } catch (cv_bridge::Exception &e) {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.",
              image_frame->encoding.c_str());
    cout<<" caught\n";
    return;
  }
  imshow("fronttt", front_image); waitKey(20);
  has_image_received = true;
}

void SimSauvcTest::enableBottom(const std_msgs::UInt8::ConstPtr &msg) {

  if (isEnabledBottom() == (msg->data))
    return;

  if (msg->data) {
    ip_status_bottom = 1;
    bottom_camera_sub =
        it.subscribe("/bottom_camera/image_rect_color", 1,
                     &SimSauvcTest::process_next_image_bottom, this);
  } else {

    ip_status_bottom = 0;
    bottom_camera_sub.shutdown();
  }
}

void SimSauvcTest::process_next_image_bottom(
    const sensor_msgs::ImageConstPtr &image_frame) {

  try {
    cv_bridge::toCvShare(image_frame, "bgr8")->image.copyTo(bottom_image);
    ROS_INFO("processing next image bottom camera");
    flip(front_image, front_image, -1);

  } catch (cv_bridge::Exception &e) {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.",
              image_frame->encoding.c_str());
    return;
  }
}

bool SimSauvcTest::isEnabledFront() { return ip_status_front; }
bool SimSauvcTest::isEnabledBottom() { return ip_status_bottom; }
bool SimSauvcTest::isEnabledGateServer() { return status_gate; }
bool SimSauvcTest::isEnabledBucketServer() { return status_bucket; }
bool SimSauvcTest::isEnabledFlareServer() { return status_flare; }
bool SimSauvcTest::isEnabledYellowFlareServer() { return status_yellow_flare; }

//--------Gate server----------------------------------------------------

void SimSauvcTest::enableGateServer(const std_msgs::UInt8::ConstPtr &msg) {
  if(has_image_received) imageProcessingGate();
  if (isEnabledGateServer() == (msg->data))
    return;

  if (msg->data) {
    status_gate = 1;
    conf_service_gate = nh.advertiseService("gate_coordinates",
                                       &SimSauvcTest::getGateCoordinates, this);
    ROS_INFO("Gate server started");
  } else {
    status_gate = 0;
    conf_service_gate.shutdown();
    ROS_INFO("Gate server shutdown");
  }
}

bool SimSauvcTest::getGateCoordinates(
    simulator_sauvc_test::Coordinates::Request &req,
    simulator_sauvc_test::Coordinates::Response &res) {

    if(req.dummy!=1){
      std::cout<<"\n## Request Denied ##\n";
      return false;
    }
    if(GT.coordinates_x.size()==0)
      return false;
    std::cout << "## Request Accepted ##" << '\n';
    res.x.push_back((GT.coordinates_x[0]+GT.coordinates_x[1])/2);
    res.y.push_back((GT.coordinates_y[0]+GT.coordinates_y[1])/2);
    res.w.push_back(abs(GT.coordinates_x[0]-GT.coordinates_x[1]));
    res.h.push_back(abs(GT.coordinates_y[0]-GT.coordinates_y[1]));
    GT.coordinates_x.clear();
    GT.coordinates_y.clear();
  return true;
}

void SimSauvcTest::imageProcessingGate() {
  cv::cvtColor(front_image,GT.hsv_frame, cv::COLOR_BGR2HSV);
  cv::blur( GT.hsv_frame,GT.gauss_frame, Size(3,3) );
  GT_green.gauss_frame=GT.gauss_frame.clone();
  cv::inRange(GT.gauss_frame, Scalar(GT.thresh_l_B, GT.thresh_l_G, GT.thresh_l_R), Scalar(GT.thresh_h_B, GT.thresh_h_G ,GT.thresh_h_R),GT.gray_frame);
  cv::inRange(GT_green.gauss_frame, Scalar(GT_green.thresh_l_B, GT_green.thresh_l_G, GT_green.thresh_l_R), Scalar(GT_green.thresh_h_B, GT_green.thresh_h_G ,GT_green.thresh_h_R),GT_green.gray_frame);
  // GT.morph_frame=GT.morph_op(GT.gray_frame);
  // GT.gray_frame=(GT.gray_frame | GT_green.gray_frame);
  cv::Canny( GT.gray_frame,GT.canny_frame,GT.canny_low_thresh,GT.canny_ratio,GT.canny_kernel_size);
  // circle(front_image,Point((min_x+max_x)/2,(min_y+max_y)/2),10,Scalar(0,0,255),2,8,0);
  imshow("server_window",GT.gray_frame);
  cv::waitKey(1);
  int min_x=1000,min_y=1000, max_x=0, max_y=0;
 //--------using hough line------------
  float a;
  vector<Vec4i> linesP; // will hold the results of the detection
  HoughLinesP(GT.canny_frame, linesP, 1, CV_PI/180,50, 50, 50 ); // runs the actual detection
  for( size_t i = 0; i < linesP.size(); i++ )
  {
      Vec4i l = linesP[i];
      if(l[2]!=l[0])
      {
        float slope =(float) ((l[3] -l[1])/(l[2]-l[0]));
        a = -1*atan(slope)*(180/CV_PI);
      }
      else a=90;

      if (abs(a)>85  || abs(a)<10){
      min_x=min(min_x,(min(l[0],l[2])));
      max_x=max(max_x,(max(l[0],l[2])));
      min_y=min(min_y,(min(l[1],l[3])));
      max_y=max(max_y,(max(l[1],l[3])));
      }
  }

  if(GT.coordinates_x.size()>0) detection_status_gate = 1;
  else detection_status_gate = 0;
  GT.coordinates_x.push_back(min_x);
  GT.coordinates_x.push_back(max_x);
  GT.coordinates_y.push_back(min_y);
  GT.coordinates_y.push_back(max_y);


  if(detection_status_gate) gate_detect_status_pub.publish(enabled);
  else gate_detect_status_pub.publish(disabled);
}

//-------------end gate server----------------------------------------------

//--------Bucket server----------------------------------------------------

void SimSauvcTest::enableBucketServer(const std_msgs::UInt8::ConstPtr &msg) {
  if(has_image_received) imageProcessingBucket();
  if (isEnabledBucketServer() == (msg->data))
    return;

  if (msg->data) {
    status_bucket = 1;
    conf_service_bucket = nh.advertiseService(
        "bucket_coordinates", &SimSauvcTest::getBucketCoordinates, this);
    ROS_INFO("Bucket server started");
  } else {
    status_bucket = 0;
    conf_service_bucket.shutdown();
    ROS_INFO("Bucket server started");
  }
}

bool SimSauvcTest::getBucketCoordinates(
    simulator_sauvc_test::Coordinates::Request &req,
    simulator_sauvc_test::Coordinates::Response &res) {

      if(req.dummy!=1){
        std::cout<<"\n## Request Denied ##\n";
        return false;
      }
      std::cout << "## Request Accepted ##" << '\n';
      for(int i=0; i<BU.coordinates_x.size(); i+=2){
        res.x.push_back((BU.coordinates_x[i]+BU.coordinates_x[i+1])/2);
        res.y.push_back((BU.coordinates_y[i]+BU.coordinates_y[i+1])/2);
        res.w.push_back(abs(BU.coordinates_x[i]-BU.coordinates_x[i+1]));
        res.h.push_back(abs(BU.coordinates_y[i]-BU.coordinates_y[i+1]));
      }
      BU.coordinates_x.clear();
      BU.coordinates_y.clear();
      return true;

}

void SimSauvcTest::imageProcessingBucket() {
  cv::cvtColor(front_image,BU.hsv_frame, cv::COLOR_BGR2HSV);
  cv::blur( BU.hsv_frame,BU.gauss_frame, Size(3,3) );
  cv::inRange(BU.gauss_frame, Scalar(BU.thresh_l_B, BU.thresh_l_G, BU.thresh_l_R), Scalar(BU.thresh_h_B, BU.thresh_h_G ,BU.thresh_h_R),BU.gray_frame);
  BU.morph_frame=BU.morph_op(BU.gray_frame);
  cv::Canny( BU.morph_frame,BU.canny_frame,BU.canny_low_thresh,BU.canny_ratio,BU.canny_kernel_size );
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  findContours(BU.canny_frame,contours, hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE);
  vector<vector<Point> > contours_poly(contours.size());
  vector<Rect> boundRect(contours.size());
  vector<Point2f> centres(contours.size());
  for(size_t i=0;i<contours.size();i++){
    approxPolyDP(contours[i],contours_poly[i],9,true);
    boundRect[i]=boundingRect(contours_poly[i]);
  }
  for(size_t i=0;i<contours.size();i++){
    if(BU.filter_points((boundRect[i].tl()).x)){
      BU.coordinates_x.push_back(boundRect[i].tl().x);
      BU.coordinates_y.push_back(boundRect[i].tl().y);
      BU.coordinates_x.push_back(boundRect[i].br().x);
      BU.coordinates_y.push_back(boundRect[i].br().y);
    }
  }
  if(BU.coordinates_x.size()>0) detection_status_bucket = 1;
  else detection_status_bucket = 0;
  BU.tl_x.clear();
  if(detection_status_bucket) bucket_detect_status_pub.publish(enabled);
  else bucket_detect_status_pub.publish(disabled);
}
//-------------end bucket server----------------------------------------------

//--------Flare server----------------------------------------------------

void SimSauvcTest::enableFlareServer(const std_msgs::UInt8::ConstPtr &msg) {
  if(has_image_received) imageProcessingFlare();
  if (isEnabledFlareServer() == (msg->data))
    return;

  if (msg->data) {
    status_flare = 1;
    conf_service_red = nh.advertiseService(
        "flare_coordinates", &SimSauvcTest::getFlareCoordinates, this);
    ROS_INFO("Flare server started");
  } else {
    status_flare = 0;
    conf_service_red.shutdown();
    ROS_INFO("Flare server shutdown");
  }
}

bool SimSauvcTest::getFlareCoordinates(
    simulator_sauvc_test::Coordinates::Request &req,
    simulator_sauvc_test::Coordinates::Response &res) {

      if(req.dummy!=1){
        std::cout<<"\n## Request Denied ##\n";
        return false;
      }
      if(RF.coordinates_x.size()==0) return false;
      std::cout << "## Request Accepted ##" << '\n';
      for(int i=0; i<RF.coordinates_x.size(); i+=2){
        res.x.push_back((RF.coordinates_x[i]+RF.coordinates_x[i+1])/2);
        res.y.push_back((RF.coordinates_y[i]+RF.coordinates_y[i+1])/2);
        res.w.push_back(abs(RF.coordinates_x[i]-RF.coordinates_x[i+1]));
        res.h.push_back(abs(RF.coordinates_y[i]-RF.coordinates_y[i+1]));
      }
      RF.coordinates_x.clear();
      RF.coordinates_y.clear();
      return true;
}

void SimSauvcTest::imageProcessingFlare() {
  cv::cvtColor(front_image,RF.hsv_frame, cv::COLOR_BGR2HSV);
  cv::blur( RF.hsv_frame,RF.gauss_frame, Size(3,3) );
  cv::inRange(RF.gauss_frame, Scalar(RF.thresh_l_B, RF.thresh_l_G, RF.thresh_l_R), Scalar(RF.thresh_h_B, RF.thresh_h_G ,RF.thresh_h_R),RF.gray_frame);
  RF.morph_frame=RF.morph_op(RF.gray_frame);
  cv::Canny( RF.morph_frame,RF.canny_frame,RF. canny_low_thresh,RF.canny_ratio,RF.canny_kernel_size );
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(RF.canny_frame,contours, hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE);
  vector<vector<Point> > contours_poly(contours.size());
  vector<Rect> boundRect(contours.size());
  vector<Point2f> centres(contours.size());
  for(size_t i=0;i<contours.size();i++){
    approxPolyDP(contours[i],contours_poly[i],9,true);
    boundRect[i]=boundingRect(contours_poly[i]);
  }
  for(size_t i=0;i<contours.size();i++){
    int diff=(boundRect[i].br().x-boundRect[i].tl().x);
    if(diff>50){
      RF.coordinates_x.push_back(boundRect[i].tl().x);
      RF.coordinates_y.push_back(boundRect[i].tl().y);
      RF.coordinates_x.push_back(boundRect[i].br().x);
      RF.coordinates_y.push_back(boundRect[i].br().y);
      rectangle(front_image,boundRect[i].tl(), boundRect[i].br(),Scalar(0,255,0));
    }
  }
  if(RF.coordinates_x.size()>0) detection_status_red_flare = 1;
  else detection_status_red_flare = 0;

  if(detection_status_red_flare) red_flare_detect_status_pub.publish(enabled);
  else red_flare_detect_status_pub.publish(disabled);
}

//-------------end flare server----------------------------------------------

// ---------- yellow_flare -------------------------------------------------
void SimSauvcTest::enableYellowFlareServer(const std_msgs::UInt8::ConstPtr &msg) {
  if(has_image_received) imageProcessingYellowFlare();
  if (isEnabledYellowFlareServer() == (msg->data))
    return;

  if (msg->data) {
    status_yellow_flare = 1;
    conf_service_yellow = nh.advertiseService(
        "yellow_flare_coordinates", &SimSauvcTest::getYellowFlareCoordinates, this);
    ROS_INFO("Yellow Flare server started");
  } else {
    status_yellow_flare = 0;
    conf_service_yellow.shutdown();
    ROS_INFO("Yellow Flare server shutdown");
  }
}

bool SimSauvcTest::getYellowFlareCoordinates(
    simulator_sauvc_test::Coordinates::Request &req,
    simulator_sauvc_test::Coordinates::Response &res) {

    if(req.dummy!=1){
      std::cout<<"\n## Request Denied ##\n";
      return false;
    }
    if(YF.coordinates_x.size()==0) return false;
    std::cout << "## Request Accepted ##" << '\n';
    cout<<"\n size: "<<YF.coordinates_x.size()<<endl;
    if(YF.coordinates_x.size()<=0) return false;
    for(int i=0; i<YF.coordinates_x.size(); i+=2){
      res.x.push_back((YF.coordinates_x[i]+YF.coordinates_x[i+1])/2);
      res.y.push_back((YF.coordinates_y[i]+YF.coordinates_y[i+1])/2);
      res.w.push_back(abs(YF.coordinates_x[i]-YF.coordinates_x[i+1]));
      res.h.push_back(abs(YF.coordinates_y[i]-YF.coordinates_y[i+1]));
    }
    YF.coordinates_x.clear();
    YF.coordinates_y.clear();
    return true;
}

void SimSauvcTest::imageProcessingYellowFlare() {
  // imshow("server_raw", front_image); waitKey(10);
  cv::cvtColor(front_image,YF.hsv_frame, cv::COLOR_BGR2HSV);
  // imshow("server_raw_hsv", YF.hsv_frame); waitKey(10);
  cv::blur( YF.hsv_frame,YF.gauss_frame, Size(3,3) );
  // imshow("server_blur", YF.gauss_frame); waitKey(10);
  cv::inRange(YF.gauss_frame, Scalar(YF.thresh_l_B, YF.thresh_l_G, YF.thresh_l_R), Scalar(YF.thresh_h_B, YF.thresh_h_G ,YF.thresh_h_R),YF.gray_frame);
  // imshow("server_raw_hsv", YF.hsv_frame); waitKey(10);
  YF.morph_frame=YF.morph_op(YF.gray_frame);
  cv::Canny( YF.morph_frame,YF.canny_frame,YF. canny_low_thresh,YF.canny_ratio,YF.canny_kernel_size );
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  findContours(YF.canny_frame,contours, hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE);
  vector<vector<Point> > contours_poly(contours.size());
  vector<Rect> boundRect(contours.size());
  vector<Point2f> centres(contours.size());
  for(size_t i=0;i<contours.size();i++){
    approxPolyDP(contours[i],contours_poly[i],9,true);
    boundRect[i]=boundingRect(contours_poly[i]);
  }
  for(size_t i=0;i<contours.size();i++){
    if(YF.filter_points((boundRect[i].tl()).x)){
      YF.coordinates_x.push_back(boundRect[i].tl().x);
      YF.coordinates_y.push_back(boundRect[i].tl().y);
      YF.coordinates_x.push_back(boundRect[i].br().x);
      YF.coordinates_y.push_back(boundRect[i].br().y);
    }
  rectangle(YF.gray_frame,boundRect[i].tl(), boundRect[i].br(),Scalar(0,255,0));
  //circle(YF.gray_frame,Point(boundRect[i].tl().x+ ,Gate_center_y),10,Scalar(0,0,255),2,8,0);
  imshow("server_yellow", YF.gray_frame);
  waitKey(10);
  }
  if(YF.coordinates_x.size()>0) detection_status_yellow_flare = 1;
  else detection_status_yellow_flare = 0;
  YF.tl_x.clear();
  if(detection_status_yellow_flare) yellow_flare_detect_status_pub.publish(enabled);
  else yellow_flare_detect_status_pub.publish(disabled);
}


// ---------- end ---------------------------------------------------------
