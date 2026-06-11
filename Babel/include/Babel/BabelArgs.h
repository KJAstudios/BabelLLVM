#ifndef BABELARGS_H
#define BABELARGS_H
#include <iostream>
#include <llvm-20/llvm/TargetParser/Host.h>
#include <llvm-20/llvm/TargetParser/Triple.h>
#include <string>
namespace Babel {
struct BabelArgs {
private:
  std::string inputFile;
  std::string outputFile;
  std::string targetTriple;
  std::string sysRoot;
  bool objectFileOnly = false;
  bool argError = false;

public:
  void SetInputFile(std::string &fileName) { inputFile = fileName; }

  std::string &GetInputFile() { return inputFile; }

  void SetOutputFile(std::string &fileName) { outputFile = fileName; }

  std::string &GetOutputFile() { return outputFile; }

  void SetTargetTriple(std::string &target) { targetTriple = target; }

  std::string &GetTargetTriple() { return targetTriple; }

  void SetSysRoot(std::string &sysroot) { sysRoot = sysroot; }

  std::string &GetSysRoot() { return sysRoot; }

  void SetObjectFileOnly() { objectFileOnly = true; }
  bool GetObjectFileOnlyStatus() const { return objectFileOnly; }

  void SetError() { argError = true; }

  bool HasError() const { return argError; }

  void Validate() {
    if (inputFile.empty()) {
      std::cerr << "No input file provided.\n";
      SetError();
      return;
    }

    if (targetTriple.empty()) {
      targetTriple = llvm::sys::getDefaultTargetTriple();
    }
    targetTriple = llvm::Triple::normalize(targetTriple);

    ValidateOutputFile();
  }

  void ValidateOutputFile() {
    if (!outputFile.empty()) {
      return;
    }
    // if no output file given, give it the same name as the output file
    outputFile = inputFile.substr(0, inputFile.size() - 4);

    // add .o if it's a output file
    if (objectFileOnly) {
      outputFile += ".o";
      return;
    }

    // if it's building for windows, add the file extension
    llvm::Triple triple(targetTriple);
    if (triple.isOSWindows()) {
      outputFile += ".exe";
    }
  }
};
} // namespace Babel
#endif