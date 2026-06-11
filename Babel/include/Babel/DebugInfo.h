#ifndef DEBUGINFO_H
#define DEBUGINFO_H
#include <llvm/IR/Argument.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <memory>
namespace Babel {
class ExpressionAST;
class StatementAST;
class FunctionAST;
struct TokenLocation;
class DebugInfo {

  static const unsigned BabelLanguageCode = 0x8111;

private:
  std::unique_ptr<llvm::DIBuilder> dwarfBuilder;
  llvm::DICompileUnit *compileUnit;
  llvm::DIType *intType;
  llvm::DIFile *Unit;
  std::string fileName;
  std::vector<llvm::DIScope *> lexicalBlocks;

public:
  explicit DebugInfo(std::string &fileName, llvm::Module &module);
  void CreateFunction(std::string &functionName, llvm::Function *function);
  void DeclareArgument(llvm::Argument &argument, llvm::AllocaInst *alloca,
                       unsigned argumentIndex, int lineNumber,
                       llvm::IRBuilder<> &builder);
  void DeclareVariable(const std::string &variableName,
                       llvm::AllocaInst *alloca, int lineNumber,
                       llvm::IRBuilder<> &builder);
  llvm::DIType *GetIntType();
  llvm::DIBuilder *GetDwarfBuilder();
  void EmitLocation(llvm::IRBuilder<> &builder);
  void EmitLocation(ExpressionAST *expression, llvm::IRBuilder<> &builder);
  void EmitLocation(StatementAST *statement, llvm::IRBuilder<> &builder);
  void EmitLocation(FunctionAST *function, llvm::IRBuilder<> &builder);
  void PopScope();

private:
  llvm::DISubroutineType *CreateFunctionType(unsigned NumArgs);
  void EmitLocation(const TokenLocation &location, llvm::IRBuilder<> &builder);
};
} // namespace Babel
#endif