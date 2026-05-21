#ifndef CODGEN_VISITOR_H
#define CODGEN_VISITOR_H
#include "Babel/Visitor.h"
#include <llvm-18/llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
namespace Babel {
class CodegenVisitor : public Visitor {
private:
// this is used to store the result of the last visited node,
               // which can be used by the parent node to generate code for
               // itself
  llvm::Value *lastResult = nullptr; 
  llvm::LLVMContext *context;

public:
  explicit CodegenVisitor(llvm::LLVMContext *context);
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
};
} // namespace Babel
#endif