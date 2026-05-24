#include "Babel/Babel.h"
#include "Babel/CodegenVisitor.h"
#include "Babel/Parser.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>
namespace Babel {
Babel::Babel() {
  context = std::make_unique<llvm::LLVMContext>();
  module = std::make_unique<llvm::Module>("Babel", *context);
  IRBuilder = std::make_unique<llvm::IRBuilder<>>(*context);
  parser = std::make_unique<Parser>(nullptr);
  codegenVisitor = std::make_unique<CodegenVisitor>(context.get(), IRBuilder.get(), module.get());
};

void Babel::Parse() { parser->Parse(); }
} // namespace Babel