#ifndef LINKER_H
#define LINKER_H
#include "Babel/BabelArgs.h"
#include <string>
namespace Babel {
class Linker {
public:
  static int RunLinker(BabelArgs babelArgs, std::string &objectFilePath, std::string &exectuablePath);

private:
  static std::string GetLibraryFilePath(std::string &executablePath, std::string &targetTriple);
  static std::string GetClangPath(std::string &executablePath);
  static std::string GetLLDPath(std::string &clangPath);
  static std::string GetSysrootPath(std::string &executablePath, std::string &targetTriple);
  static int RemoveObjectFile(std::string &objectFilePath);
};
} // namespace Babel
#endif
