#ifndef MEDIAPIPE_CALCULATORS_UTIL_DETECTION_ID_TO_FILE_CALCULATOR_H_
#define MEDIAPIPE_CALCULATORS_UTIL_DETECTION_ID_TO_FILE_CALCULATOR_H_

#include "mediapipe/framework/calculator_framework.h"

namespace mediapipe {

// Calculator for writing detection IDs to a text file.
class DetectionIdToFileCalculator : public CalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract* cc);
  absl::Status Open(CalculatorContext* cc) override;
  absl::Status Process(CalculatorContext* cc) override;

 private:
  std::string file_path_;  // Path to the output text file.
};

}  // namespace mediapipe

#endif  // MEDIAPIPE_CALCULATORS_UTIL_DETECTION_ID_TO_FILE_CALCULATOR_H_
