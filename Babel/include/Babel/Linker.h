#ifndef LINKER_H
#define LINKER_H
#include "Babel/BabelArgs.h"
#include <string>
namespace Babel {
class Linker {
public:
  static void RunLinker(BabelArgs babelArgs, std::string &objectFilePath, std::string &exectuablePath);

private:
  static std::string GetLibraryFilePath();
  static std::string GetClangPath(std::string &executablePath);
};
} // namespace Babel
#endif
