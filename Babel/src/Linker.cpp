#include "Babel/Linker.h"
#include "Babel/BabelArgs.h"
#include <filesystem>
#include <llvm/Support/Path.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/Program.h>
#include <string>
#include <system_error>
namespace Babel {
void Linker::RunLinker(BabelArgs babelArgs, std::string &objectFilePath,
                       std::string &executablePath) {

  if (!std::filesystem::exists("runtime.bc")) {
    std::cerr << "Babel Runtime Library not found";
    return;
  }

  std::string clangPath = GetClangPath(executablePath);
  if(clangPath.empty()){
	std::cerr << "Error: Clang not found.";
  }

  std::string libraryFilePath = GetLibraryFilePath();
  std::string target;
  std::string sysroot;
  std::vector<llvm::StringRef> args = {clangPath,
                                       objectFilePath,
                                       libraryFilePath,
                                       "-o",
                                       babelArgs.GetOutputFile(),
                                       "-Wno-override-module"};

  if (!babelArgs.GetTargetTriple().empty()) {
    target = "--target=" + babelArgs.GetTargetTriple();
    args.emplace_back(target);
  }

  if (!babelArgs.GetSysRoot().empty()) {
    sysroot = "--sysroot=" + babelArgs.GetSysRoot();
    args.emplace_back(target);
  }

  std::string errorMessage;
  int result = llvm::sys::ExecuteAndWait(clangPath, args, std::nullopt, {}, 0,
                                         0, &errorMessage);

  std::error_code errorCode = llvm::sys::fs::remove(objectFilePath);
  if (errorCode) {
    std::cerr << "Error cleaning up temporary object file: "
              << errorCode.message() << '\n';
  }
}

std::string Linker::GetLibraryFilePath() {
  if (llvm::sys::fs::exists("runtime.bc")) {
    return "runtime.bc";
  }

  return "dependencies/runtime.bc";
}

std::string Linker::GetClangPath(std::string &executablePath) {
  llvm::SmallString<256> exePath(executablePath);
  llvm::sys::path::remove_filename(exePath);
  llvm::sys::path::append(exePath, "dependencies");

  llvm::SmallVector<llvm::StringRef, 4> candidates;
#if defined(_WIN32)
  candidates = {"clang.exe"};
#else
  candidates = {"clang-20", "clang"};
#endif

  // check the build folder for the dependency
  for (auto &name : candidates) {
    llvm::SmallString<256> bundledPath(exePath);
    llvm::sys::path::append(bundledPath, name);
    if (llvm::sys::fs::exists(bundledPath)) {
      return std::string(bundledPath);
    }
  }

  // check PATH if the dependecy isn't found
  for (auto &name : candidates) {
    auto found = llvm::sys::findProgramByName(name);
    if (found) {
      return *found;
    }
  }

  return "";
}
} // namespace Babel