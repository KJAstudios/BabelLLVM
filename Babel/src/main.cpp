#include "Babel//Linker.h"
#include "Babel/ArgumentParser.h"
#include "Babel/Babel.h"
#include "Babel/BabelArgs.h"
#include <llvm/ADT/SmallString.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Program.h>
#include <system_error>

namespace {
std::string GetOutputFile(Babel::BabelArgs &args) {
  if (args.GetObjectFileOnlyStatus()) {
    return args.GetOutputFile();
  }

  llvm::SmallString<256> tempFilePath;
  std::error_code errorCode =
      llvm::sys::fs::createTemporaryFile("babelOutput", "o", tempFilePath);
  if (errorCode) {
    std::cerr << "Error creating temporary object file: " << errorCode.message()
              << '\n';
    return "";
  }

  return std::string(tempFilePath.str());
}
} // namespace

int main(int argCount, char *argv[]) {
  std::vector<std::string> args(argv, argv + argCount);
  Babel::BabelArgs argData = Babel::ArgumentParser::ParseArgs(args);
  if (argData.HasError()) {
    return 1;
  }

  Babel::Babel babel = Babel::Babel();
  int runResult = babel.SetupModuleForTarget(argData.GetTargetTriple());
  if (runResult != 0) {
    return runResult;
  }

  runResult = babel.Run(argData);
  if (runResult != 0) {
    return runResult;
  }

  std::string outputFileName = GetOutputFile(argData);
  if (outputFileName.empty()) {
    return 1;
  }

  babel.OutputObjectFile(outputFileName);
  // don't run the linker if the object file flag is set
  if (argData.GetObjectFileOnlyStatus()) {
    llvm::errs() << "Object file written to " << outputFileName << '\n';
    return 0;
  }

  // get the location of the executable so we can grab clang bundled with the
  // build
  std::string executablePath =
      llvm::sys::fs::getMainExecutable(argv[0], nullptr);

  Babel::Linker::RunLinker(argData, outputFileName, executablePath);
  llvm::errs() << "Executable written to " << argData.GetOutputFile() << '\n';
};
