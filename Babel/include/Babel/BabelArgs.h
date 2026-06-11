#ifndef BABELARGS_H
#define BABELARGS_H
#include <iostream>
#include <string>
namespace Babel {
struct BabelArgs {
private:
  std::string inputFile;
  std::string outputFile;
  std::string targetTriple;
  bool objectFileOnly = false;
  bool argError = false;

public:
  void SetInputFile(std::string &fileName) { inputFile = fileName; }

  std::string &GetInputFile() { return inputFile; }

  void SetOutputFile(std::string &fileName) { outputFile = fileName; }

  std::string &GetOutputFile() { return outputFile; }

  void SetTargetTriple(std::string &target) { targetTriple = target; }

  std::string &GetTargetTriple() { return targetTriple; }

  void SetObjectFileOnly() { objectFileOnly = true; }
  bool GetObjectFileOnlyStatus() const { return objectFileOnly; }

  void SetError() { argError = true; }

  bool HasError() const { return argError; }

  void Validate() {
    if (inputFile.empty()) {
      SetError();
      return;
    }

    if (outputFile.empty()) {
      std::cerr << "output file set to " << inputFile << '\n';
      outputFile = inputFile.substr(0, inputFile.size() - 4) + ".o";
    }
  }
};
} // namespace Babel
#endif