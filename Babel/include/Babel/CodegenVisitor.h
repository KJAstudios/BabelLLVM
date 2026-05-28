#ifndef CODGEN_VISITOR_H
#define CODGEN_VISITOR_H
#include "Babel/ScopeStack.h"
#include "Babel/Visitor.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
namespace Babel {
class CodegenVisitor : public Visitor {
private:
  // this is used to store the result of the last visited node,
  // which can be used by the parent node to generate code for
  // itself
  llvm::Value *lastResult = nullptr;
  bool statementGenerationFailed = false;
  llvm::LLVMContext *context;
  llvm::IRBuilder<> *builder;
  llvm::Module *module;
  std::unique_ptr<ScopeStack> scopeStack;

public:
  CodegenVisitor(llvm::LLVMContext *context,
                          llvm::IRBuilder<> *builder, llvm::Module *module);
  void VisitProgram(ProgramAST *program) override;
  void VisitStatementBlock(StatementBlockAST *statmentBlock) override;
  void VisitPrototype(PrototypeAST *prototype) override;
  void VisitFunction(FunctionAST *function) override;
  void
  VisitFunctionCallStatement(FunctionCallStatementAST *functionCall) override;
  void VisitIfStatement(IfStatementAST *ifStatement) override;
  void VisitAssignmentStatement(
      AssignmentStatementAST *assignmentStatement) override;
  void
  VisitVariableExpression(VariableExpressionAST *variableExpression) override;
  void VisitIntExpression(IntExpressionAST *intExpression) override;
  void VisitDoubleExpression(DoubleExpressionAST *doubleExpression) override;
  void VisitBinaryExpression(BinaryExpressionAST *binaryExpression) override;

private:
  llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *function,
                                           std::string variableName);
};
} // namespace Babel
#endif