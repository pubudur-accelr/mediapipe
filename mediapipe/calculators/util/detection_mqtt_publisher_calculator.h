#ifndef MEDIAPIPE_CALCULATORS_UTIL_DETECTION_MQTT_PUBLISHER_CALCULATOR_H_
#define MEDIAPIPE_CALCULATORS_UTIL_DETECTION_MQTT_PUBLISHER_CALCULATOR_H_

#include "mediapipe/framework/calculator_framework.h"
#include <mosquitto.h>
#include <string>

namespace mediapipe {

class DetectionMQTTPublisherCalculator : public CalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract* cc);
  absl::Status Open(CalculatorContext* cc) override;
  absl::Status Process(CalculatorContext* cc) override;

 private:
  std::string broker_address_;
  std::string username_;
  std::string password_;
  std::string topic_;
  mosquitto* mosq_;

  static void on_connect(mosquitto* mosq, void* userdata, int rc);
  static void on_publish(mosquitto* mosq, void* userdata, int mid);
};

REGISTER_CALCULATOR(DetectionMQTTPublisherCalculator);

}  // namespace mediapipe

#endif  // MEDIAPIPE_CALCULATORS_UTIL_DETECTION_MQTT_PUBLISHER_CALCULATOR_H_
