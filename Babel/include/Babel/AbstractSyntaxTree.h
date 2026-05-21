#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H
#include <iostream>
#include <llvm-18/llvm/ADT/StringRef.h>
#include <memory>
#include <string>
#include <vector>
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

// statement block is a sequence of statements that are executed in order. This
// is currently only used for the blocks of for statements, but will need to be
// expanded on
class StatementBlockAST : public StatementAST {
protected:
  std::vector<std::unique_ptr<StatementAST>> body;

public:
  StatementBlockAST() { std::cerr << "Created statement block\n"; }
  void AddStatement(std::unique_ptr<StatementAST> statement) {
    body.push_back(std::move(statement));
  }
};

class PrototypeAST {
private:
  std::string name;
  std::vector<std::string> args;

public:
  PrototypeAST() = delete;
  explicit PrototypeAST(std::string name, std::vector<std::string> args)
      : name(std::move(name)), args(std::move(args)) {
    std::cerr << "Created prototype with name " << this->name << " and "
              << this->args.size() << " arguments\n";
  }
  std::string *GetName() { return &name; }
};

class FunctionAST {
private:
  std::unique_ptr<PrototypeAST> prototype;
  std::unique_ptr<StatementBlockAST> body;

public:
  FunctionAST() = delete;
  explicit FunctionAST(std::unique_ptr<PrototypeAST> prototype,
                       std::unique_ptr<StatementBlockAST> body)
      : prototype(std::move(prototype)), body(std::move(body)) {
    std::cerr << "Created function with name " << this->prototype->GetName()->c_str() << "\n";
  }
};

class FunctionCallStatementAST : public StatementAST {
private:
  std::string functionName;
  std::vector<std::unique_ptr<ExpressionAST>> arguments;

public:
  FunctionCallStatementAST() = delete;
  explicit FunctionCallStatementAST(
      std::string functionName,
      std::vector<std::unique_ptr<ExpressionAST>> arguments)
      : functionName(std::move(functionName)), arguments(std::move(arguments)) {
    std::cerr << "Created function call statement with name "
              << this->functionName << " and " << this->arguments.size()
              << " arguments\n";
  }
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
        elseBranch(std::move(elseBranch)) {
    std::cerr << "Created if statement\n";
  }
};

class AssignmentStatementAST : public StatementAST {
private:
  std::string name;
  // todo: add type information
  std::unique_ptr<ExpressionAST> initializer;

public:
  AssignmentStatementAST(std::string name,
                         std::unique_ptr<ExpressionAST> initializer)
      : name(std::move(name)), initializer(std::move(initializer)) {
    std::cerr << "Created assignment statement with name " << this->name
              << "\n";
  }
};

class VariableExpressionAST : public ExpressionAST {
private:
  std::string name;

public:
  VariableExpressionAST() = delete;
  explicit VariableExpressionAST(std::string name) : name(std::move(name)) {
    std::cerr << "Created variable expression with name " << this->name << "\n";
  }
};

class IntExpressionAST : public ExpressionAST {
private:
  int value;

public:
  IntExpressionAST() = delete;
  explicit IntExpressionAST(int value) : value(value) {
    std::cerr << "Created int expression with value " << this->value << "\n";
  }
};

class DoubleExpressionAST : public ExpressionAST {
private:
  double value;
};

class BinaryExpressionAST : public ExpressionAST {
private:
  std::string binaryOperator;
  std::unique_ptr<ExpressionAST> lhs;
  std::unique_ptr<ExpressionAST> rhs;

public:
  BinaryExpressionAST() = delete;
  BinaryExpressionAST(std::string binaryOperator,
                      std::unique_ptr<ExpressionAST> lhs,
                      std::unique_ptr<ExpressionAST> rhs)
      : binaryOperator(std::move(binaryOperator)), lhs(std::move(lhs)),
        rhs(std::move(rhs)) {
    std::cerr << "Created binary expression with operator "
              << this->binaryOperator << "\n";
  }
};

} // namespace Babel
#endif