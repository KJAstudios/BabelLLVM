#include "Babel/DebugInfo.h"
#include "Babel/AbstractSyntaxTree.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/BinaryFormat/Dwarf.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>

namespace Babel {

DebugInfo::DebugInfo(std::string &inputFileName, llvm::Module &module)
    : dwarfBuilder(std::make_unique<llvm::DIBuilder>(module)),
      fileName(inputFileName),
      compileUnit(dwarfBuilder->createCompileUnit(
          llvm::dwarf::DW_LANG_C, dwarfBuilder->createFile(inputFileName, "."),
          "Babel Compiler", false, "", 0)),
      Unit(dwarfBuilder->createFile(compileUnit->getFilename(),
                                    compileUnit->getDirectory())),
      intType(nullptr) {}

void DebugInfo::CreateFunction(std::string &functionName,
                               llvm::Function *function) {
  llvm::DIScope *fileContext = Unit;
  unsigned lineNumber = 0;
  unsigned scopeLine = 0;
  llvm::DISubprogram *subProgram = dwarfBuilder->createFunction(
      fileContext, functionName, llvm::StringRef(), Unit, lineNumber,
      CreateFunctionType(function->arg_size()), scopeLine,
      llvm::DINode::FlagPrototyped, llvm::DISubprogram::SPFlagDefinition);
  function->setSubprogram(subProgram);
  lexicalBlocks.push_back(subProgram);
}

void DebugInfo::DeclareArgument(llvm::Argument &argument,
                                llvm::AllocaInst *alloca,
                                unsigned argumentIndex, int lineNumber,
                                llvm::IRBuilder<> &builder) {
  llvm::DILocalVariable *localVariable = dwarfBuilder->createParameterVariable(
      lexicalBlocks.back(), argument.getName(), argumentIndex, Unit, lineNumber,
      GetIntType(), true);
  dwarfBuilder->insertDeclare(
      alloca, localVariable, dwarfBuilder->createExpression(),
      llvm::DILocation::get(lexicalBlocks.back()->getContext(), lineNumber, 0,
                            lexicalBlocks.back()),
      builder.GetInsertBlock());
}

void DebugInfo::DeclareVariable(const std::string &variableName,
                                llvm::AllocaInst *alloca, int lineNumber,
                                llvm::IRBuilder<> &builder) {
  llvm::DILocalVariable *localVariable = dwarfBuilder->createAutoVariable(
      lexicalBlocks.back(), variableName, Unit, lineNumber, GetIntType(), true);
  dwarfBuilder->insertDeclare(
      alloca, localVariable, dwarfBuilder->createExpression(),
      llvm::DILocation::get(lexicalBlocks.back()->getContext(), lineNumber, 0,
                            lexicalBlocks.back()),
      builder.GetInsertBlock());
}

void DebugInfo::PopScope() { lexicalBlocks.pop_back(); }

llvm::DIType *DebugInfo::GetIntType() {
  if (intType != nullptr) {
    return intType;
  }

  intType =
      dwarfBuilder->createBasicType("int", 64, llvm::dwarf::DW_ATE_signed);
  return intType;
}

llvm::DIBuilder *DebugInfo::GetDwarfBuilder() { return dwarfBuilder.get(); }

llvm::DISubroutineType *DebugInfo::CreateFunctionType(unsigned NumArgs) {
  llvm::SmallVector<llvm::Metadata *, 8> elementTypes;
  llvm::DIType *intType = GetIntType();

  elementTypes.push_back(intType);

  for (unsigned i = 0, e = NumArgs; i != e; ++i) {
    elementTypes.push_back(intType);
  }

  return dwarfBuilder->createSubroutineType(
      dwarfBuilder->getOrCreateTypeArray(elementTypes));
}

void DebugInfo::EmitLocation(llvm::IRBuilder<> &builder) {
  builder.SetCurrentDebugLocation(llvm::DebugLoc());
}

void DebugInfo::EmitLocation(ExpressionAST *expression,
                             llvm::IRBuilder<> &builder) {
  if (expression == nullptr) {
    builder.SetCurrentDebugLocation(llvm::DebugLoc());
    return;
  }
  EmitLocation(expression->GetLocation(), builder);
}

void DebugInfo::EmitLocation(StatementAST *statement,
                             llvm::IRBuilder<> &builder) {
  if (statement == nullptr) {
    builder.SetCurrentDebugLocation(llvm::DebugLoc());
    return;
  }
  EmitLocation(statement->GetLocation(), builder);
}

void DebugInfo::EmitLocation(FunctionAST *function,
                             llvm::IRBuilder<> &builder) {
  if (function == nullptr) {
    builder.SetCurrentDebugLocation(llvm::DebugLoc());
    return;
  }
  EmitLocation(function->GetLocation(), builder);
}

void DebugInfo::EmitLocation(const TokenLocation &location,
                             llvm::IRBuilder<> &builder) {
  llvm::DIScope *Scope = nullptr;
  if (lexicalBlocks.empty()) {
    Scope = compileUnit;
  } else {
    Scope = lexicalBlocks.back();
  }
  builder.SetCurrentDebugLocation(llvm::DILocation::get(
      Scope->getContext(), location.GetLine(), location.GetColumn(), Scope));
}
} // namespace Babel