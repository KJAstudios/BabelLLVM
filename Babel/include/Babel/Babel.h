#ifndef BABEL_H
#define BABEL_H
#include "CodegenVisitor.h"
#include "Babel/Parser.h"
#include <memory>
#include <llvm/IR/IRBuilder.h>
namespace Babel {
class Babel {
private:
  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> IRBuilder;
  std::unique_ptr<Parser> parser;
  std::unique_ptr<CodegenVisitor> codegenVisitor;

public:
	Babel();
	void Parse();
};
} // namespace Babel
#endif