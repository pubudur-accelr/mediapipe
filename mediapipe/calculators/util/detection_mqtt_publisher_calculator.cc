#include "mediapipe/calculators/util/detection_mqtt_publisher_calculator.pb.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/packet.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/status_macros.h"
#include <mosquitto.h>
#include <iostream>

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

absl::Status DetectionMQTTPublisherCalculator::GetContract(
    CalculatorContract* cc) {
  cc->Inputs().Index(0).Set<std::vector<Detection>>();
  return absl::OkStatus();
}

absl::Status DetectionMQTTPublisherCalculator::Open(CalculatorContext* cc) {
  const auto& options = cc->Options<DetectionMQTTPublisherCalculatorOptions>();
  broker_address_ = options.broker_address();
  username_ = options.username();
  password_ = options.password();
  topic_ = options.topic();

  mosquitto_lib_init();

  mosq_ = mosquitto_new(NULL, true, this);
  if (!mosq_) {
    return absl::InternalError("Failed to create Mosquitto client.");
  }

  mosquitto_username_pw_set(mosq_, username_.c_str(), password_.c_str());

  int rc = mosquitto_connect(mosq_, broker_address_.c_str(), 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    return absl::InternalError("Failed to connect to MQTT broker.");
  }

  mosquitto_connect_callback_set(mosq_, &DetectionMQTTPublisherCalculator::on_connect);
  mosquitto_publish_callback_set(mosq_, &DetectionMQTTPublisherCalculator::on_publish);

  mosquitto_loop_start(mosq_);

  return absl::OkStatus();
}

absl::Status DetectionMQTTPublisherCalculator::Process(CalculatorContext* cc) {
  if (cc->Inputs().Index(0).IsEmpty()) {
    return absl::OkStatus();
  }
  const auto& detections = cc->Inputs().Index(0).Get<std::vector<Detection>>();

  for (const Detection& detection : detections) {
    std::string message;  // Construct message from detection
    
    // Check if detection has text labels
    if (!detection.label().empty()) {
      for (const std::string& label : detection.label()) {
        message += label + "\n";  // Add label to message
      }
    }
    // If no text labels, use label IDs
    else if (!detection.label_id().empty()) {
      for (int label_id : detection.label_id()) {
        message += std::to_string(label_id) + "\n";  // Convert label ID to string and add to message
      }
    }

    // Publish message to MQTT topic
    int rc = mosquitto_publish(mosq_, NULL, topic_.c_str(), message.size(), message.c_str(), 0, false);
    if (rc != MOSQ_ERR_SUCCESS) {
      return absl::InternalError("Failed to publish message to MQTT topic.");
    }
  }

  return absl::OkStatus();
}

void DetectionMQTTPublisherCalculator::on_connect(mosquitto* mosq, void* userdata, int rc) {
  if (rc == 0) {
    LOG(INFO) << "Connected to MQTT broker.";
  } else {
    LOG(ERROR) << "Failed to connect to MQTT broker. Error code: " << rc;
  }
}

void DetectionMQTTPublisherCalculator::on_publish(mosquitto* mosq, void* userdata, int mid) {
  LOG(INFO) << "Message published to MQTT topic.";
}

}  // namespace mediapipe
