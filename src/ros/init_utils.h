#ifndef __INIT_UTILS_H_INCLUDED
#define __INIT_UTILS_H_INCLUDED

#include <string>
#include <memory>

#include <ros/ros.h>

#include "../core/world.h"
#include "topic_with_transform.h"
#include "pose_correction_tf_publisher.h"
#include "occupancy_grid_publisher.h"

std::string tf_odom_frame_id() {
  std::string id;
  ros::param::param<std::string>("~tf_odom_frame_id", id, "odom_combined");
  return id;
}

std::string tf_map_frame_id() {
  std::string id;
  ros::param::param<std::string>("~tf_map_frame_id", id, "map");
  return id;
}

bool is_async_correction() {
  bool async_correction;
  ros::param::param<bool>("~async_correction", async_correction, false);
  return async_correction;
}

bool init_skip_exceeding_lsr() {
  bool param_value;
  ros::param::param<bool>("~skip_exceeding_lsr_vals", param_value, false);
  return param_value;
}

template <typename ObservT, typename MapT>
std::shared_ptr<PoseCorrectionTfPublisher<ObservT>>
create_pose_correction_tf_publisher(WorldObservable<MapT> *slam,
                                    TopicWithTransform<ObservT> *scan_prov) {
  auto pose_publisher = std::make_shared<PoseCorrectionTfPublisher<ObservT>>(
    tf_map_frame_id(), tf_odom_frame_id(), is_async_correction()
  );
  scan_prov->subscribe(pose_publisher);
  slam->subscribe_pose(pose_publisher);
  return pose_publisher;
}

template <typename MapT>
std::shared_ptr<OccupancyGridPublisher<MapT>>
create_occupancy_grid_publisher(WorldObservable<MapT> *slam,
                                ros::NodeHandle nh,
                                double ros_map_publishing_rate) {
  auto map_publisher = std::make_shared<OccupancyGridPublisher<MapT>>(
    nh.advertise<nav_msgs::OccupancyGrid>("map", 5),
    tf_map_frame_id(), ros_map_publishing_rate);
  slam->subscribe_map(map_publisher);
  return map_publisher;
}

#endif
