#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H
#include "Babel/CodegenVisitor.h"
#include "Babel/Token.h"
#include "Babel/TokenData.h"
#include "Babel/Visitor.h"
#include <iostream>
#include <llvm/ADT/StringRef.h>
#include <memory>
#include <string>
#include <vector>
namespace Babel {
class StatementAST {
private:
  TokenLocation location;

public:
  StatementAST() = default;
  explicit StatementAST(TokenLocation location) : location(location) {}
  StatementAST(int line, int column) : location(line, column) {}
  StatementAST(const StatementAST &) = default;
  StatementAST(StatementAST &&) = delete;
  StatementAST &operator=(const StatementAST &) = default;
  StatementAST &operator=(StatementAST &&) = delete;
  virtual ~StatementAST() = default;
  virtual void Visit(Visitor &visitor) = 0;
  const TokenLocation &GetLocation() { return location; }
};

class ExpressionAST {
private:
  TokenLocation location;

public:
  ExpressionAST() = default;
  explicit ExpressionAST(TokenLocation location) : location(location) {}
  ExpressionAST(int line, int column) : location(line, column) {}
  ExpressionAST(const ExpressionAST &) = default;
  ExpressionAST(ExpressionAST &&) = delete;
  ExpressionAST &operator=(const ExpressionAST &) = default;
  ExpressionAST &operator=(ExpressionAST &&) = delete;
  virtual ~ExpressionAST() = default;
  virtual void Visit(Visitor &visitor) = 0;
  const TokenLocation &GetLocation() { return location; }
};

// statement block is a sequence of statements that are executed in order. This
// is currently only used for the blocks of for statements, but will need to be
// expanded on
class StatementBlockAST : public StatementAST {
private:
  std::vector<std::unique_ptr<StatementAST>> body;

public:
  StatementBlockAST() { std::cerr << "Created statement block\n"; }
  explicit StatementBlockAST(TokenLocation location) : StatementAST(location) {
    std::cerr << "Created statement block\n";
  }
  StatementBlockAST(int line, int column) : StatementAST(line, column) {
    std::cerr << "Created statement block\n";
  }
  void AddStatement(std::unique_ptr<StatementAST> statement) {
    body.push_back(std::move(statement));
  }
  std::vector<std::unique_ptr<StatementAST>> *GetBody() { return &body; }
  void Visit(class Visitor &visitor) override {
    visitor.VisitStatementBlock(this);
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
  std::vector<std::string> *GetArgs() { return &args; }
  void Visit(class Visitor &visitor) { visitor.VisitPrototype(this); }
};

class FunctionAST {
private:
  std::unique_ptr<PrototypeAST> prototype;
  std::unique_ptr<StatementBlockAST> body;
  TokenLocation tokenLocation;

public:
  FunctionAST() = delete;
  explicit FunctionAST(std::unique_ptr<PrototypeAST> prototype,
                       std::unique_ptr<StatementBlockAST> body)
      : prototype(std::move(prototype)), body(std::move(body)) {
    std::cerr << "Created function with name "
              << this->prototype->GetName()->c_str() << "\n";
  }
  explicit FunctionAST(TokenLocation location,
                       std::unique_ptr<PrototypeAST> prototype,
                       std::unique_ptr<StatementBlockAST> body)
      : tokenLocation(location), prototype(std::move(prototype)),
        body(std::move(body)) {
    std::cerr << "Created function with name "
              << this->prototype->GetName()->c_str() << "\n";
  }
  PrototypeAST *GetPrototype() { return prototype.get(); }
  StatementBlockAST *GetBody() { return body.get(); }
  const TokenLocation &GetLocation() { return tokenLocation; }
  void Visit(class Visitor &visitor) { visitor.VisitFunction(this); }
};

class ProgramAST {
private:
  std::vector<std::unique_ptr<FunctionAST>> programFunctions;

public:
  void AddFunction(std::unique_ptr<FunctionAST> function) {
    programFunctions.push_back(std::move(function));
  }
  const std::vector<std::unique_ptr<FunctionAST>> *GetFunctions() {
    return &programFunctions;
  }
  void Visit(CodegenVisitor &visitor) { visitor.VisitProgram(this); }
};

class FunctionCallStatementAST : public StatementAST {
private:
  std::string functionName;
  std::vector<std::unique_ptr<ExpressionAST>> arguments;

public:
  FunctionCallStatementAST() = delete;
  explicit FunctionCallStatementAST(
      TokenLocation location, std::string functionName,
      std::vector<std::unique_ptr<ExpressionAST>> arguments)
      : StatementAST(location), functionName(std::move(functionName)),
        arguments(std::move(arguments)) {
    std::cerr << "Created function call statement with name "
              << this->functionName << " and " << this->arguments.size()
              << " arguments\n";
  }
  explicit FunctionCallStatementAST(
      std::string functionName,
      std::vector<std::unique_ptr<ExpressionAST>> arguments)
      : functionName(std::move(functionName)), arguments(std::move(arguments)) {
    std::cerr << "Created function call statement with name "
              << this->functionName << " and " << this->arguments.size()
              << " arguments\n";
  }
  std::string *GetName() { return &functionName; }
  std::vector<std::unique_ptr<ExpressionAST>> *GetArguments() {
    return &arguments;
  }
  void Visit(class Visitor &visitor) override {
    visitor.VisitFunctionCallStatement(this);
  }
};

class IfStatementAST : public StatementAST {
private:
  std::unique_ptr<ExpressionAST> condition;
  std::unique_ptr<StatementAST> thenBranch;
  std::unique_ptr<StatementAST> elseBranch;

public:
  IfStatementAST(TokenLocation location,
                 std::unique_ptr<ExpressionAST> condition,
                 std::unique_ptr<StatementAST> thenBranch,
                 std::unique_ptr<StatementAST> elseBranch)
      : StatementAST(location), condition(std::move(condition)),
        thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {
    std::cerr << "Created if statement\n";
  }
  IfStatementAST(std::unique_ptr<ExpressionAST> condition,
                 std::unique_ptr<StatementAST> thenBranch,
                 std::unique_ptr<StatementAST> elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {
    std::cerr << "Created if statement\n";
  }
  ExpressionAST *GetCondition() { return condition.get(); }
  StatementAST *GetThenBranch() { return thenBranch.get(); }
  StatementAST *GetElseBranch() { return elseBranch.get(); }
  void Visit(class Visitor &visitor) override {
    visitor.VisitIfStatement(this);
  }
};

class AssignmentStatementAST : public StatementAST {
private:
  std::string name;
  // todo: add type information
  std::unique_ptr<ExpressionAST> initializer;

public:
  AssignmentStatementAST(TokenLocation location, std::string name,
                         std::unique_ptr<ExpressionAST> initializer)
      : StatementAST(location), name(std::move(name)),
        initializer(std::move(initializer)) {
    std::cerr << "Created assignment statement with name " << this->name
              << "\n";
  }
  AssignmentStatementAST(std::string name,
                         std::unique_ptr<ExpressionAST> initializer)
      : name(std::move(name)), initializer(std::move(initializer)) {
    std::cerr << "Created assignment statement with name " << this->name
              << "\n";
  }
  std::string GetName() { return name; }
  ExpressionAST *GetInitalizer() { return initializer.get(); }
  void Visit(class Visitor &visitor) override {
    visitor.VisitAssignmentStatement(this);
  }
};

class VariableExpressionAST : public ExpressionAST {
private:
  std::string name;

public:
  VariableExpressionAST() = delete;
  VariableExpressionAST(TokenLocation location, std::string name)
      : ExpressionAST(location), name(std::move(name)) {
    std::cerr << "Created variable expression with name " << this->name << "\n";
  }
  explicit VariableExpressionAST(std::string name) : name(std::move(name)) {
    std::cerr << "Created variable expression with name " << this->name << "\n";
  }
  std::string *GetName() { return &name; }
  void Visit(class Visitor &visitor) override {
    visitor.VisitVariableExpression(this);
  }
};

class IntExpressionAST : public ExpressionAST {
private:
  int value;

public:
  IntExpressionAST() = delete;
  IntExpressionAST(TokenLocation location, int value)
      : ExpressionAST(location), value(value) {
    std::cerr << "Created int expression with value " << this->value << "\n";
  }
  explicit IntExpressionAST(int value) : value(value) {
    std::cerr << "Created int expression with value " << this->value << "\n";
  }
  int GetValue() { return value; }
  void Visit(class Visitor &visitor) override {
    visitor.VisitIntExpression(this);
  }
};

class DoubleExpressionAST : public ExpressionAST {
private:
  double value;

public:
  DoubleExpressionAST() = delete;
  DoubleExpressionAST(TokenLocation location, double value)
      : ExpressionAST(location), value(value) {
    std::cerr << "Created double expression with value " << this->value << "\n";
  }
  explicit DoubleExpressionAST(double value) : value(value) {
    std::cerr << "Created double expression with value " << this->value << "\n";
  }
  double GetValue() { return value; }
  void Visit(class Visitor &visitor) override {
    visitor.VisitDoubleExpression(this);
  }
};

class BinaryExpressionAST : public ExpressionAST {
private:
  std::string binaryOperator;
  std::unique_ptr<ExpressionAST> lhs;
  std::unique_ptr<ExpressionAST> rhs;

public:
  BinaryExpressionAST() = delete;
  BinaryExpressionAST(TokenLocation location, std::string binaryOperator,
                      std::unique_ptr<ExpressionAST> lhs,
                      std::unique_ptr<ExpressionAST> rhs)
      : ExpressionAST(location), binaryOperator(std::move(binaryOperator)),
        lhs(std::move(lhs)), rhs(std::move(rhs)) {
    std::cerr << "Created binary expression with operator "
              << this->binaryOperator << "\n";
  }
  BinaryExpressionAST(std::string binaryOperator,
                      std::unique_ptr<ExpressionAST> lhs,
                      std::unique_ptr<ExpressionAST> rhs)
      : binaryOperator(std::move(binaryOperator)), lhs(std::move(lhs)),
        rhs(std::move(rhs)) {
    std::cerr << "Created binary expression with operator "
              << this->binaryOperator << "\n";
  }
  std::string *GetBinaryOperator() { return &binaryOperator; }
  ExpressionAST *GetLHS() { return lhs.get(); }
  ExpressionAST *GetRHS() { return rhs.get(); }
  void Visit(class Visitor &visitor) override {
    visitor.VisitBinaryExpression(this);
  }
};

} // namespace Babel
#endif