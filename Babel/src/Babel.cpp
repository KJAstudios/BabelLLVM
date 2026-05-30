#include "Babel/Babel.h"
#include "Babel/AbstractSyntaxTree.h"
#include "Babel/BabelArgs.h"
#include "Babel/CodegenVisitor.h"
#include "Babel/Parser.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
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
  codegenVisitor = std::make_unique<CodegenVisitor>(
      context.get(), IRBuilder.get(), module.get());
};

int Babel::Run(BabelArgs args) {
  parser = std::make_unique<Parser>(args.GetInputFile());
  std::unique_ptr<ProgramAST> program = parser->Parse();
  program->Visit(*codegenVisitor);
  module->print(llvm::errs(), nullptr);
  return OutputProgram(args.GetOutputFile());
}

int Babel::OutputProgram(std::string *fileName) {
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
    return 1;
  }

  // create the target machine
  llvm::TargetOptions options;
  llvm::TargetMachine *targetMachine =
      target->createTargetMachine(targetTriple, "generic", "", options, {});

  // set the data layout
  module->setDataLayout(targetMachine->createDataLayout());

  std::string validationError;
  llvm::raw_string_ostream errorStream(validationError);
  if (llvm::verifyModule(*module, &errorStream)) {
    llvm::errs() << "Module verification failed:\n" << validationError << "\n";
    return 1;
  }

  // emit to file
  std::string outputFile = *fileName + ".o";
  std::error_code errorCode;
  llvm::raw_fd_ostream destination(outputFile, errorCode);
  if (errorCode) {
    llvm::errs() << "Could not create or open output object file " << outputFile << ": "
                 << errorCode.message() << "\n";
    return 1;
  }

  llvm::legacy::PassManager passManager;
  if (targetMachine->addPassesToEmitFile(passManager, destination, nullptr,
                                         llvm::CodeGenFileType::ObjectFile)) {
    llvm::errs() << "Target Machine cannot emit an object file";
    return 1;
  }
  passManager.run(*module);
  destination.flush();
  llvm::errs() << "object file written to " << outputFile << '\n';
  return 0;
}
} // namespace Babel