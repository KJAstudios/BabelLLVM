#include "Babel/Babel.h"
#include "Babel/BabelArgs.h"
#include <filesystem>
#include <llvm/Support/Program.h>

namespace {
bool IsValidBabelFileName(const std::string &fileName) {
  return std::filesystem::path(fileName).extension() == ".bbl";
};

Babel::BabelArgs ParseArgs(std::vector<std::string> &args) {
  Babel::BabelArgs babelArgs = Babel::BabelArgs();
  if (args.size() == 1 || args.size() > 4) {
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

    // currently the only option the output file
    if (curArg == "-o") {
      if (i == args.size() - 1) {
        std::cerr << "No output file given";
        babelArgs.SetError();
        return babelArgs;
      }
      // the next argument is the output file
      i++;
      babelArgs.SetOutputFile(args[i]);
      // this is the only current option, so we can stop looking at arguments
      break;
    }
  }

  babelArgs.Validate();
  return babelArgs;
}

void RunLinker(std::string &outputFile) {
  std::string clangPath = llvm::sys::findProgramByName("clang").get();
  std::string objectFileName = outputFile + ".o";

  if(!std::filesystem::exists("runtime.bc")){
    std::cerr << "Babel Runtime Library not found";
    return;
  }

  std::vector<llvm::StringRef> args = {clangPath, objectFileName, "runtime.bc", "-o",
                                       outputFile};

  std::string errorMessage;
  int result = llvm::sys::ExecuteAndWait(clangPath, args, std::nullopt, {}, 0,
                                         0, &errorMessage);
}
} // namespace

int main(int argCount, char *argv[]) {
  std::vector<std::string> args(argv, argv + argCount);
  Babel::BabelArgs argData = ParseArgs(args);
  if (argData.HasError()) {
    return 1;
  }

  Babel::Babel babel = Babel::Babel();
  babel.Run(argData);
  RunLinker(*argData.GetOutputFile());
};
