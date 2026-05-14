#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H
#include <llvm-18/llvm/ADT/StringRef.h>
#include <memory>
#include <string>
namespace Babel {
class StatementAST {
public:
  StatementAST() = default;
  StatementAST(const StatementAST &) = default;
  StatementAST(StatementAST &&) = delete;
  StatementAST &operator=(const StatementAST &) = default;
  StatementAST &operator=(StatementAST &&) = delete;
  virtual ~StatementAST() = default;
};

class ExpressionAST {
public:
  ExpressionAST() = default;
  ExpressionAST(const ExpressionAST &) = default;
  ExpressionAST(ExpressionAST &&) = delete;
  ExpressionAST &operator=(const ExpressionAST &) = default;
  ExpressionAST &operator=(ExpressionAST &&) = delete;
  virtual ~ExpressionAST() = default;
};

class IfStatementAST : public StatementAST {
private:
  std::unique_ptr<ExpressionAST> condition;
  std::unique_ptr<StatementAST> thenBranch;
  std::unique_ptr<StatementAST> elseBranch;

public:
  IfStatementAST(std::unique_ptr<ExpressionAST> condition,
                 std::unique_ptr<StatementAST> thenBranch,
                 std::unique_ptr<StatementAST> elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
};

class AssignmentStatementAST : public StatementAST {
private:
  llvm::StringRef name;
  // todo: add type information
  std::unique_ptr<ExpressionAST> initializer;

public:
  AssignmentStatementAST(llvm::StringRef name,
                         std::unique_ptr<ExpressionAST> initializer)
      : name(name), initializer(std::move(initializer)) {}
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
  std::unique_ptr<ExpressionAST> lhs;
  std::unique_ptr<ExpressionAST> rhs;
};

} // namespace Babel
#endif