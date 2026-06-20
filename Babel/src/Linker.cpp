#include "Babel/Linker.h"
#include "Babel/BabelArgs.h"
#include <filesystem>
#include <llvm/ADT/SmallString.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/Program.h>
#include <string>
#include <system_error>
namespace Babel {
int Linker::RunLinker(BabelArgs babelArgs, std::string &objectFilePath,
                      std::string &executablePath) {
  std::string clangPath = GetClangPath(executablePath);
  if (clangPath.empty()) {
    std::cerr << "Error: Clang not found.\n";
    RemoveObjectFile(objectFilePath);
    return 1;
  }

  std::string libraryFilePath = GetLibraryFilePath(executablePath);
  if (libraryFilePath.empty()) {
    std::cerr << "Error: core library not found.\n";
    RemoveObjectFile(objectFilePath);
    return 1;
  }

  // define before the args declaration to avoid bad memory due to how StringRef works
  std::string target;
  std::string sysroot;
  std::string lldPath;
  std::vector<llvm::StringRef> args = {clangPath,
                                       objectFilePath,
                                       libraryFilePath,
                                       "-o",
                                       babelArgs.GetOutputFile(),
                                       "-Wno-override-module"};

  bool targetDefined = !babelArgs.GetTargetTriple().empty();

  if (targetDefined) {
    target = "--target=" + babelArgs.GetTargetTriple();
    args.emplace_back(target);

    // if a custom target is defined, ensure we use the bundled lld
    lldPath = GetLLDPath(clangPath);
    if(lldPath.empty()){
      std::cerr << "Error: lld not found.\n";
      RemoveObjectFile(objectFilePath);
      return 1;
    }
    lldPath = "--ld-path=" + lldPath;
    args.emplace_back(lldPath);

    // ensure lld uses the included runtime library
    args.emplace_back("-rtlib=compiler-rt");
    args.emplace_back("--unwindlib=libunwind");
  }

  if (!babelArgs.GetSysRoot().empty()) {
    sysroot = "--sysroot=" + babelArgs.GetSysRoot();
    args.emplace_back(sysroot);
  }
  // default to the packaged dependencies if a target is provided and no sysroot
  else if (targetDefined) {
    sysroot = "--sysroot=" +
              GetSysrootPath(executablePath, babelArgs.GetTargetTriple());
    args.emplace_back(sysroot);
  }

  std::cerr << "Clang Args: ";
  for (auto arg : args) {
    std::cerr << arg.str() << " ";
  }
  std::cerr << '\n';

  std::string errorMessage;
  int result = llvm::sys::ExecuteAndWait(clangPath, args, std::nullopt, {}, 0,
                                         0, &errorMessage);
  if (result != 0) {
    std::cerr << "Clang linker failed (exit " << result << "): " << errorMessage
              << "\n";
    RemoveObjectFile(objectFilePath);
    return 1;
  }

  return RemoveObjectFile(objectFilePath);
}

std::string Linker::GetLibraryFilePath(std::string &executablePath) {
  // case for running in the development environment
  if (llvm::sys::fs::exists("runtime.bc")) {
    return "runtime.bc";
  }

  // dependencies folder is only copied over for the finalized build
  llvm::SmallString<256> libPath(executablePath);
  llvm::sys::path::remove_filename(libPath);
  llvm::sys::path::append(libPath, "dependencies", "runtime.bc");

  if (llvm::sys::fs::exists(libPath)) {
    return std::string(libPath);
  }

  return "";
}

std::string Linker::GetClangPath(std::string &executablePath) {
  llvm::SmallString<256> exePath(executablePath);
  llvm::sys::path::remove_filename(exePath);
  llvm::sys::path::append(exePath, "dependencies", "bin");

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
    std::cerr << "Checking for clang at path: " << bundledPath.str().str()
              << "\n";
    if (llvm::sys::fs::exists(bundledPath)) {
      std::cerr << "Clang found.\n";
      return std::string(bundledPath);
    }
  }

  std::cerr << "Clang not found in dependencies\n";
  // check PATH if the dependecy isn't found
  for (auto &name : candidates) {
    auto found = llvm::sys::findProgramByName(name);
    if (found) {
      return *found;
    }
  }

  return "";
}

std::string Linker::GetLLDPath(std::string &clangPath) {
  llvm::SmallString<256> lldPath(clangPath);
  llvm::sys::path::remove_filename(lldPath);

  llvm::StringRef lldName;
#if defined(_WIN32)
  lldName = {"ld.lld.exe"};
#else
  lldName = {"ld.lld"};
#endif

  llvm::sys::path::append(lldPath, lldName);
  std::cerr << "Checking for lld at path: " << lldPath.str().str() << "\n";
  if (llvm::sys::fs::exists(lldPath)) {
    std::cerr << "lld found.\n";
    return std::string(lldPath);
  }

  std::cerr << "lld not found in dependencies\n";
  // check PATH if the dependecy isn't found
  auto found = llvm::sys::findProgramByName(lldName);
  if (found) {
    return *found;
  }

  return "";
}

std::string Linker::GetSysrootPath(std::string &executablePath,
                                   std::string &targetTriple) {
  llvm::SmallString<256> sysrootPath(executablePath);
  llvm::sys::path::remove_filename(sysrootPath);
  llvm::sys::path::append(sysrootPath, "dependencies", "sysroots",
                          targetTriple);
  return sysrootPath.str().str();
}

int Linker::RemoveObjectFile(std::string &objectFilePath) {
  std::error_code errorCode = llvm::sys::fs::remove(objectFilePath);
  if (errorCode) {
    std::cerr << "Error cleaning up temporary object file: "
              << errorCode.message() << '\n';
    return 1;
  }
  return 0;
}
} // namespace Babel