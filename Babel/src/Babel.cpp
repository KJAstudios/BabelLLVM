#include "Babel/Babel.h"
#include "Babel/AbstractSyntaxTree.h"
#include "Babel/CodegenVisitor.h"
#include "Babel/Parser.h"
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <memory>
#include <system_error>

namespace Babel {
Babel::Babel() {
  context = std::make_unique<llvm::LLVMContext>();
  module = std::make_unique<llvm::Module>("Babel", *context);
  IRBuilder = std::make_unique<llvm::IRBuilder<>>(*context);
  parser = std::make_unique<Parser>(nullptr);
  codegenVisitor = std::make_unique<CodegenVisitor>(
      context.get(), IRBuilder.get(), module.get());
};

void Babel::Run() {
  std::unique_ptr<ProgramAST> program = parser->Parse();
  program->Visit(*codegenVisitor);
}

void Babel::OutputProgram() {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  // get the target triple data
  auto targetTriple = llvm::sys::getDefaultTargetTriple();

  std::string error;
  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(targetTriple, error);
  if (target == nullptr) {
    std::cerr << "Failed to find requested build target: " << error << '\n';
    return;
  }

  // create the target machine
  llvm::TargetOptions options;
  llvm::TargetMachine *targetMachine =
      target->createTargetMachine(targetTriple, "generic", "", options, {});

  // set the data layout
  module->setDataLayout(targetMachine->createDataLayout());

  // emit to file
  std::error_code errorCode;
  llvm::raw_fd_ostream destination("output.o", errorCode);
  llvm::legacy::PassManager passManager;
  targetMachine->addPassesToEmitFile(passManager, destination, nullptr,
                                     llvm::CodeGenFileType::ObjectFile);
  passManager.run(*module);
  destination.flush();
}
} // namespace Babel