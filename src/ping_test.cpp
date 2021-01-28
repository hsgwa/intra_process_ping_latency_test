// Copyright 2021 hsgwa
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("test_node");

  auto option = rclcpp::SubscriptionOptions();
  option.topic_stats_options.state = rclcpp::TopicStatisticsState::Enable;
  option.topic_stats_options.publish_period = 1s;
  option.topic_stats_options.publish_topic = "/topic_statistics";

  auto sub = node->create_subscription<sensor_msgs::msg::Imu>(
    "topic", 10,
    [](sensor_msgs::msg::Imu::UniquePtr msg) {
      (void) msg;
    },
    option);

  auto pub = node->create_publisher<sensor_msgs::msg::Imu>("topic", 10);

  auto timer = node->create_wall_timer(
    100ms,
    [&pub, &node]() {
      static int i = 0;
      auto msg = std::make_unique<sensor_msgs::msg::Imu>();
      if (i++ < 100) {
        msg->header.stamp = node->now() - 1s;
        // Imitation of transition states
        RCLCPP_INFO(node->get_logger(), "Publishing: transition state message. ");
      } else {
        msg->header.stamp = node->now();
        RCLCPP_INFO(node->get_logger(), "Publishing: steady state message");
      }
      pub->publish(std::move(msg));
    });

  rclcpp::executors::SingleThreadedExecutor executor;

  executor.add_node(node);
  executor.spin();

  rclcpp::shutdown();
  return 0;
}
