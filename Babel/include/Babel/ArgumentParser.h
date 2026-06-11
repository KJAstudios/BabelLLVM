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
void ParseTarget(std::string &target);
void ParseSysRoot(std::string &sysroot);
static bool IsValidBabelFileName(const std::string &fileName);
};
} // namespace Babel
#endif