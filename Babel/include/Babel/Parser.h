#ifndef PARSER_H
#define PARSER_H
#include "Babel/AbstractSyntaxTree.h"
#include "Babel/Lexer.h"
#include "Babel/Token.h"
#include <memory>
namespace Babel {
class Parser {
private:
  std::unique_ptr<Lexer> lexer;
  Token tokenState;

public:
  Parser() = delete;
  explicit Parser(std::string *filename = nullptr);
  void Parse();
  std::unique_ptr<StatementAST> ParseStatement();
  std::unique_ptr<ExpressionAST> ParseExpression();

private:
  Babel::Token GetNextToken();
  std::unique_ptr<StatementAST> ParseIfStatement();
  std::unique_ptr<StatementAST> ParseAssignmentStatement();
};
} // namespace Babel
#endif