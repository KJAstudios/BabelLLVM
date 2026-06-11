#ifndef BABEL_H
#define BABEL_H
#include "Babel/DebugInfo.h"
#include "Babel/Parser.h"
#include "BabelArgs.h"
#include "CodegenVisitor.h"
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>

namespace Babel {
class Babel {
private:
  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> IRBuilder;
  std::unique_ptr<Parser> parser;
  std::unique_ptr<CodegenVisitor> codegenVisitor;
  std::unique_ptr<DebugInfo> debugInfo;
  std::unique_ptr<llvm::TargetMachine> targetMachine;

public:
  Babel();
  int SetupModuleForTarget(std::string &targetTriple);
  int Run(BabelArgs &args);
  int OutputObjectFile(std::string &fileName);

private:
  bool DoesMainExist();
};
} // namespace Babel
#endif