#ifndef DEBUGINFO_H
#define DEBUGINFO_H
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <memory>
namespace Babel {
class DebugInfo {

	static const unsigned BabelLanguageCode = 0x8111;
private:
  std::unique_ptr<llvm::DIBuilder> dwarfBuilder;
  llvm::DICompileUnit *compileUnit;
  llvm::DIType *intType;
  std::string fileName;


public:
  explicit DebugInfo(std::string &fileName);
  llvm::DIType *GetIntType();
  llvm::DIBuilder *GetDwarfBuilder();
};
} // namespace Babel
#endif