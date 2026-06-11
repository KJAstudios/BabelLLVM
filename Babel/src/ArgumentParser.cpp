#include "Babel/ArgumentParser.h"
#include "Babel/BabelArgs.h"
#include <cstddef>
#include <filesystem>
namespace Babel {
BabelArgs ArgumentParser::ParseArgs(std::vector<std::string> &args) {
  babelArgs = Babel::BabelArgs();
  if (args.size() < 2) {
    std::cerr << "No input file provided.";
    return babelArgs;
  }

  // skip arg[0], since that's the program name
  for (size_t i = 1; i < args.size(); i++) {
    std::string &curArg = args[i];

    // only output object file
    if (curArg == "-c") {
      babelArgs.SetObjectFileOnly();
      continue;
    }

    // name output file
    if (curArg == "-o") {
      if (i == args.size() - 1) {
        std::cerr << "No output file given.";
        babelArgs.SetError();
        return babelArgs;
      }
      // the next argument is the output file
      babelArgs.SetOutputFile(args[++i]);
      continue;
    }

    // build target
    if (curArg == "-target") {
      if (i == args.size() - 1) {
        std::cerr << "No target provided.";
        babelArgs.SetError();
        return babelArgs;
      }
      babelArgs.SetTargetTriple(args[++i]);
      continue;
    }

    if (babelArgs.GetInputFile().empty() && IsValidBabelFileName(curArg)) {

      babelArgs.SetInputFile(curArg);
      continue;
    }

    std::cerr << "Invalid Argument " << curArg << '\n';
    babelArgs.SetError();
    return babelArgs;
  }

  babelArgs.Validate();
  return babelArgs;
}

bool ArgumentParser::IsValidBabelFileName(const std::string &fileName) {
  return std::filesystem::path(fileName).extension() == ".bbl";
};
} // namespace Babel