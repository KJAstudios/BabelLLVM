#ifndef PARSER_H
#define PARSER_H
#include "Babel/AbstractSyntaxTree.h"
#include "Babel/Lexer.h"
#include "Babel/Token.h"
#include <map>
#include <memory>
#include <string>
namespace Babel {
class Parser {
private:
  std::unique_ptr<Lexer> lexer;
  Token tokenState;
  std::map<std::string, int> operatorPrecedence;

public:
  Parser() = delete;
  explicit Parser(std::string *filename = nullptr);
  std::unique_ptr<ProgramAST> Parse();
  std::unique_ptr<StatementAST> ParseStatement();
  std::unique_ptr<ExpressionAST> ParseExpression();

private:
  void InitOperatorPrecedence();
  void GetNextToken();
  std::unique_ptr<StatementAST> ParseIfStatement();
  std::unique_ptr<StatementAST> ParseAssignmentStatement(std::string identifier);
  std::unique_ptr<StatementAST> ParseStatementBlock();
  std::unique_ptr<StatementAST> ParseIdentifierStatement();
  std::unique_ptr<FunctionAST> ParseFunction();
  std::unique_ptr<StatementAST> ParseFunctionCall(std::string functionName);
  std::unique_ptr<ExpressionAST> ParsePrimary();
  std::unique_ptr<ExpressionAST>
  ParseBinaryOpRHS(int precedence, std::unique_ptr<ExpressionAST> leftHandSide);
  std::unique_ptr<ExpressionAST> ParseIdentifierExpression();
  std::unique_ptr<ExpressionAST> ParseNumberExpression();
  bool CheckEndOfStatement();
  bool CheckEndOfFunction();
};
} // namespace Babel
#endif