#include "Babel/ArgumentParser.h"
#include "Babel/BabelArgs.h"
#include <filesystem>
#include <llvm/TargetParser/Triple.h>
#include <string>
namespace Babel {
  BabelArgs Babel::ArgumentParser::babelArgs = {};
BabelArgs ArgumentParser::ParseArgs(std::vector<std::string> &args) {
  if (args.size() < 2) {
    std::cerr << "No input file provided.";
    return babelArgs;
  }

  // skip arg[0], since that's the program name
  for (size_t i = 1; i < args.size(); i++) {
    std::string curArg = args[i];

    // only output object file
    // doesn't need more parsing, so we just set the flag
    if (curArg == "-c") {
      babelArgs.SetObjectFileOnly();
      continue;
    }

    if (curArg.starts_with("--")) {
      if (!ParseSingleArg(curArg)) {
        return babelArgs;
      }
      continue;
    }

    if (curArg.starts_with("-")) {
      if (i == args.size() - 1) {
        std::cerr << "No argument found for " << curArg << '\n';
        babelArgs.SetError();
        return babelArgs;
      }
      if (args[i + 1].starts_with("-")) {
        std::cerr << "No operator found for flag " << curArg << '\n';
        return babelArgs;
      }
      if (!ParseDoubleArg(
              std::pair<std::string, std::string>(curArg, args[i + 1]))) {
        return babelArgs;
      }

      // skip the next arg since it was used already
      i++;
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

bool ArgumentParser::ParseDoubleArg(
    std::pair<std::string, std::string> doubleArg) {
  if (doubleArg.first == "-o") {
    if (!babelArgs.GetOutputFile().empty()) {
      std::cerr << "output file already set\n";
      babelArgs.SetError();
      return false;
    }
    babelArgs.SetOutputFile(doubleArg.second);
    return true;
  }

  if (doubleArg.first == "-target") {
    if (!babelArgs.GetTargetTriple().empty()) {
      std::cerr << "Target already set\n";
      babelArgs.SetError();
      return false;
    }
    babelArgs.SetTargetTriple(doubleArg.second);
    return true;
  }

  if (doubleArg.first == "-sysroot") {
    if (!babelArgs.GetSysRoot().empty()) {
      std::cerr << "sysroot already set\n";
      babelArgs.SetError();
      return false;
    }
    babelArgs.SetSysRoot(doubleArg.second);
    return true;
  }

  std::cerr << "Invalid flag " << doubleArg.first << '\n';
  babelArgs.SetError();
  return false;
}

bool ArgumentParser::ParseSingleArg(std::string arg) {
  std::string prefix = "--target=";
  std::string output;

  if (arg.starts_with(prefix)) {
    if (!babelArgs.GetTargetTriple().empty()) {
      std::cerr << "Target already set\n";
      babelArgs.SetError();
      return false;
    }
    output = arg.substr(prefix.size());
    babelArgs.SetTargetTriple(output);
    return true;
  }

  prefix = "--sysroot=";
  if (arg.starts_with(prefix)) {
    if (!babelArgs.GetSysRoot().empty()) {
      std::cerr << "sysroot already set\n";
      babelArgs.SetError();
      return false;
    }
    output = arg.substr(prefix.size());
    babelArgs.SetSysRoot(output);
    return true;
  }

  std::cerr << "Invalid flag " << arg << '\n';
  babelArgs.SetError();
  return false;
}

bool ArgumentParser::IsValidBabelFileName(const std::string &fileName) {
  return std::filesystem::path(fileName).extension() == ".bbl";
};
} // namespace Babel