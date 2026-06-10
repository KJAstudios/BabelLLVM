#include "Babel/Babel.h"
#include "Babel/BabelArgs.h"
#include <filesystem>
#include <llvm-20/llvm/ADT/SmallString.h>
#include <llvm-20/llvm/Support/FileSystem.h>
#include <llvm/Support/Program.h>
#include <system_error>

namespace {
bool IsValidBabelFileName(const std::string &fileName) {
  return std::filesystem::path(fileName).extension() == ".bbl";
};

Babel::BabelArgs ParseArgs(std::vector<std::string> &args) {
  Babel::BabelArgs babelArgs = Babel::BabelArgs();
  if (args.size() == 1 || args.size() > 5) {
    std::cerr << "Invalid number of arguments";
    return babelArgs;
  }

  // skip arg[0], since that's the program name
  for (int i = 1; i < args.size(); i++) {
    std::string &curArg = args[i];
    // only check for input file in the first argument
    if (i == 1) {

      if (IsValidBabelFileName(curArg)) {
        babelArgs.SetInputFile(curArg);
        continue;
      }

      std::cerr << curArg << " is an invalid Babel file. Must end in .bbl\n";
      babelArgs.SetError();
      return babelArgs;
    }

    // only output object file
    if (curArg == "-c") {
      babelArgs.SetObjectFileOnly();
      continue;
    }

    // name output file
    if (curArg == "-o") {
      if (i == args.size() - 1) {
        std::cerr << "No output file given";
        babelArgs.SetError();
        return babelArgs;
      }
      // the next argument is the output file
      babelArgs.SetOutputFile(args[++i]);
      continue;
    }
  }

  babelArgs.Validate();
  return babelArgs;
}

void RunLinker(std::string &outputFile, std::string &objectFilePath) {
  std::string clangPath = llvm::sys::findProgramByName("clang").get();

  if (!std::filesystem::exists("runtime.bc")) {
    std::cerr << "Babel Runtime Library not found";
    return;
  }

  std::vector<llvm::StringRef> args = {clangPath, objectFilePath, "runtime.bc",
                                       "-o", outputFile};

  std::string errorMessage;
  int result = llvm::sys::ExecuteAndWait(clangPath, args, std::nullopt, {}, 0,
                                         0, &errorMessage);

  std::error_code errorCode = llvm::sys::fs::remove(objectFilePath);
  if (errorCode) {
    std::cerr << "Error cleaning up temporary object file: "
              << errorCode.message() << '\n';
  }
}

std::string GetOutputFile(Babel::BabelArgs &args) {
  if (args.GetObjectFileOnlyStatus()) {
    return *args.GetOutputFile();
  }

  llvm::SmallString<256> tempFilePath;
  std::error_code errorCode =
      llvm::sys::fs::createTemporaryFile("babelOutput", "o", tempFilePath);
  if (errorCode) {
    std::cerr << "Error creating temporary object file: " << errorCode.message()
              << '\n';
    return "";
  }

  return tempFilePath.c_str();
}
} // namespace

int main(int argCount, char *argv[]) {
  std::vector<std::string> args(argv, argv + argCount);
  Babel::BabelArgs argData = ParseArgs(args);
  if (argData.HasError()) {
    return 1;
  }

  Babel::Babel babel = Babel::Babel();
  int runResult = babel.Run(argData);
  if (runResult != 0) {
    return runResult;
  }

  std::string outputFileName = GetOutputFile(argData);
  if (outputFileName.empty()) {
    return 1;
  }

  babel.OutputObjectFile(&outputFileName);
  // don't run the linker if the object file flag is set
  if (argData.GetObjectFileOnlyStatus()) {
    llvm::errs() << "object file written to " << outputFileName << '\n';
    return 0;
  }

  RunLinker(*argData.GetOutputFile(), outputFileName);
  llvm::errs() << "executable written to " << *argData.GetOutputFile() << '\n';
};
