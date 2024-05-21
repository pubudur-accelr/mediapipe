#include "mediapipe/calculators/util/detection_id_to_file_calculator.pb.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include <fstream>
#include <string>

namespace mediapipe {

class DetectionIdToFileCalculator : public CalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract* cc);
  absl::Status Open(CalculatorContext* cc) override;
  absl::Status Process(CalculatorContext* cc) override;

 private:
  std::string output_file_path_;
  bool append_mode_;
};

REGISTER_CALCULATOR(DetectionIdToFileCalculator);

absl::Status DetectionIdToFileCalculator::GetContract(CalculatorContract* cc) {
  cc->Inputs().Index(0).Set<std::vector<Detection>>();  // Expects a vector of Detections
  return absl::OkStatus();
}

absl::Status DetectionIdToFileCalculator::Open(CalculatorContext* cc) {
  const auto& options = cc->Options<DetectionIdToFileCalculatorOptions>();
  output_file_path_ = options.output_file_path();
  append_mode_ = options.append_mode();
  return absl::OkStatus();
}

absl::Status DetectionIdToFileCalculator::Process(CalculatorContext* cc) {
  if (cc->Inputs().Index(0).IsEmpty()) {
    return absl::OkStatus();
  }
  const auto& detections = cc->Inputs().Index(0).Get<std::vector<Detection>>();
  std::ofstream file(output_file_path_, append_mode_ ? std::ios_base::app : std::ios_base::trunc);

  for (const Detection& detection : detections) {
    // Write text labels if they exist
    if (!detection.label().empty()) {
      for (const std::string& label : detection.label()) {
        file << label << "\n";
      }
    }
    // Fallback to writing label IDs if no text labels are present
    else if (!detection.label_id().empty()) {
      for (int label_id : detection.label_id()) {
        file << label_id << "\n";
      }
    }
  }

  file.close();
  return absl::OkStatus();
}

}  // namespace mediapipe
