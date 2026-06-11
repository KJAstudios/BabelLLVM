#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H
#include "Babel/BabelArgs.h"
#include <string>
#include <vector>
namespace Babel {
class ArgumentParser {
private:
  static BabelArgs babelArgs;
  

public:
   static BabelArgs ParseArgs(std::vector<std::string> &args);

private:
  
   static bool IsValidBabelFileName(const std::string &fileName);
   static bool ParseDoubleArg(std::pair<std::string, std::string> doubleArg);
   static bool ParseSingleArg(std::string arg);
};
} // namespace Babel
#endif