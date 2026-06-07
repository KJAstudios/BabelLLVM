#ifndef DEBUGINFO_H
#define DEBUGINFO_H
#include <llvm-20/llvm/IR/Metadata.h>
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
  llvm::DIFile *Unit;
  std::string fileName;


public:
  explicit DebugInfo(std::string &fileName);
  void CreateFunction(std::string &functionName, llvm::Function *function);
  llvm::DIType *GetIntType();
  llvm::DIBuilder *GetDwarfBuilder();

  private:
  llvm::DISubroutineType *CreateFunctionType(unsigned NumArgs);
  
};
} // namespace Babel
#endif