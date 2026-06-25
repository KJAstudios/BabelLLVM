#include "Babel/ArgumentParser.h"
#include "Babel/BabelArgs.h"
#include <filesystem>
#include <llvm/TargetParser/Triple.h>
#include <string>
namespace Babel {
  BabelArgs Babel::ArgumentParser::babelArgs = {};
BabelArgs ArgumentParser::ParseArgs(std::vector<std::string> &args) {

  if (args.size() < 2) {
    PrintHelp();
    babelArgs.SetError();
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

    // only print help if --help or -h is found
    if(curArg == "-h" || curArg == "--help"){
      PrintHelp();
      // set error so program doesn't continue
      babelArgs.SetError();
      return babelArgs;
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

void ArgumentParser::PrintHelp(){
  std::cerr << "Usage: babel <.bbl source file> [OPTIONS]\n\n";
  std::cerr << "Option                Long option             Meaning\n";
  std::cerr << "-h                    --help                  Display program help\n";
  std::cerr << "-c                                            Output object file instead of executable\n";
  std::cerr << "-o <file>                                     Define name of output file\n";
  std::cerr << "-target <value>       --target=<value>        Build program for the given target\n";
  std::cerr << "-sysroot <directory>  --sysroot=<value>       Define the directory to search for system headers\n";
  std::cerr << "Currently supported build targets are: x86_64-unknown-linux-gnu, aarch64-unknown-linux-gnu, and x86_64-w64-windows-gnu";
}
} // namespace Babel