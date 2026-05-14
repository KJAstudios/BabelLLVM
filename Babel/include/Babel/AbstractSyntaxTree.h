#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H
#include <llvm-18/llvm/ADT/StringRef.h>
#include <memory>
#include <string>
namespace Babel {
class StatementAST {
public:
  StatementAST(const StatementAST &) = default;
  StatementAST(StatementAST &&) = delete;
  StatementAST &operator=(const StatementAST &) = default;
  StatementAST &operator=(StatementAST &&) = delete;
  virtual ~StatementAST() = default;
};

class ExpressionAST {
public:
  ExpressionAST(const ExpressionAST &) = default;
  ExpressionAST(ExpressionAST &&) = delete;
  ExpressionAST &operator=(const ExpressionAST &) = default;
  ExpressionAST &operator=(ExpressionAST &&) = delete;
  virtual ~ExpressionAST() = default;
};

class IfSatementAST : public StatementAST {
private:
  std::unique_ptr<ExpressionAST> *condition;
  std::unique_ptr<StatementAST> *thenBranch;
  std::unique_ptr<StatementAST> *elseBranch;
};

class AssignmentSatementAST : public StatementAST {
private:
  std::string name;
  // todo: add type information
  std::unique_ptr<ExpressionAST> *initializer;
};

class IntExpressionAST : public ExpressionAST {
private:
  int value;
};

class DoubleExpressionAST : public ExpressionAST {
private:
  double value;
};

class BinaryExpressionAST : public ExpressionAST {
private:
  llvm::StringRef binaryOperator;
  std::unique_ptr<ExpressionAST> *lhs;
  std::unique_ptr<ExpressionAST> *rhs;
};

} // namespace Babel
#endif