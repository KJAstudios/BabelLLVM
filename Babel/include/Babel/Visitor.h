#ifndef VISITOR_H
#define VISITOR_H

namespace Babel {
class StatementBlockAST;
class PrototypeAST;
class FunctionAST;
class FunctionCallStatementAST;
class IfStatementAST;
class AssignmentStatementAST;
class VariableExpressionAST;
class IntExpressionAST;
class DoubleExpressionAST;
class BinaryExpressionAST;
class Visitor {
public:
  Visitor() = default;
  Visitor(const Visitor &) = default;
  Visitor(Visitor &&) = delete;
  Visitor &operator=(const Visitor &) = default;
  Visitor &operator=(Visitor &&) = delete;
  virtual ~Visitor() = default;
  virtual void VisitStatementBlock(StatementBlockAST *statmentBlock) = 0;
  virtual void VisitPrototype(PrototypeAST *prototype) = 0;
  virtual void VisitFunction(FunctionAST *function) = 0;
  virtual void
  VisitFunctionCallStatement(FunctionCallStatementAST *functionCall) = 0;
  virtual void VisitIfStatement(IfStatementAST *ifStatement) = 0;
  virtual void
  VisitAssignmentStatement(AssignmentStatementAST *assignmentStatement) = 0;
  virtual void
  VisitVariableExpression(VariableExpressionAST *variableExpression) = 0;
  virtual void VisitIntExpression(IntExpressionAST *intExpression) = 0;
  virtual void VisitDoubleExpression(DoubleExpressionAST *doubleExpression) = 0;
  virtual void VisitBinaryExpression(BinaryExpressionAST *binaryExpression) = 0;
};
} // namespace Babel
#endif