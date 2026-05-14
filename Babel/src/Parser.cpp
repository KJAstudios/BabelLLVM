#include "Babel/Parser.h"
#include "Babel/AbstractSyntaxTree.h"
#include "Babel/Token.h"
#include <iostream>
#include <llvm-18/llvm/ADT/StringRef.h>
#include <memory>
namespace Babel {
Babel::Parser::Parser(std::string *filename) : tokenState(Token::tok_empty) {
  lexer = std::make_unique<Lexer>();
  lexer->LoadBuffer(filename);
}
Babel::Token Parser::GetNextToken() {
  tokenState = lexer->GetNextToken();
  return tokenState;
}

void Parser::Parse() {
  Babel::Token token = GetNextToken();
  switch (token) {
  case Token::tok_eof:
    return;
  case Token::tok_if:
  case Token::tok_else:
  case Token::tok_for:
  case Token::tok_identifier:
    Parser::ParseStatement();
    break;
  default:
    std::cerr << "Token not supported as a starting statement or expression\n";
    return;
  }
}

std::unique_ptr<StatementAST> Parser::ParseStatement() {
  switch (tokenState) {
  case Token::tok_if:
    return ParseIfStatement();
  case Token::tok_identifier:
    return ParseAssignmentStatement();
  default:
    return nullptr;
  }
}

std::unique_ptr<StatementAST> Parser::ParseIfStatement() {
  Babel::Token token = GetNextToken(); // consume the if token

  if (token != Token::tok_control ||
      lexer->GetControlCharacter()->str() != "⟅") {
    std::cerr << "Expected ⟅ after if statement\n";
    return nullptr;
  }

  token = GetNextToken();
  if (token != Token::tok_number || token != Token::tok_identifier) {
    std::cerr << "Expected condition after if statement\n";
    return nullptr;
  }
  // parse condition expression
  std::unique_ptr<ExpressionAST> condition = ParseExpression();

  if (tokenState != Token::tok_control ||
      lexer->GetControlCharacter()->str() != "⟆") {
    std::cerr << "Expected ⟆ after if statement condition\n";
    return nullptr;
  }

  token = GetNextToken(); // consume the ⟆
  if (token != Token::tok_control ||
      lexer->GetControlCharacter()->str() != "꧁") {
    std::cerr << "Expected ꧁ before if statement then branch\n";
    return nullptr;
  }

  std::unique_ptr<StatementAST> thenBranch = ParseStatement();

  if (tokenState != Token::tok_control ||
      lexer->GetControlCharacter()->str() != "꧂") {
    std::cerr << "Expected ꧂ after if statement then branch\n";
    return nullptr;
  }

  token = GetNextToken(); // consume the ꧂
  std::unique_ptr<StatementAST> elseBranch = nullptr;
  if (token == Token::tok_else) {
    token = GetNextToken();
    if (token != Token::tok_control ||
        lexer->GetControlCharacter()->str() != "꧁") {
      std::cerr << "Expected ꧁ before if statement else branch\n";
      return nullptr;
    }

    elseBranch = ParseStatement();

    if (tokenState != Token::tok_control ||
        lexer->GetControlCharacter()->str() != "꧂") {
      std::cerr << "Expected ꧂ after if statement else branch\n";
      return nullptr;
    }

    token = GetNextToken(); // consume the ꧂
  }

  return std::make_unique<IfStatementAST>(
      std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<StatementAST> Parser::ParseAssignmentStatement() {
  llvm::StringRef *identifier = lexer->GetIdentifierStr();
  Babel::Token token = GetNextToken(); // consume the identifier token
  if (token != Token::tok_operator || lexer->GetOperatorStr()->str() != "≔") {
    std::cerr << "Expected ≔ after identifier in assignment statement\n";
    return nullptr;
  }

  token = GetNextToken(); // consume the ≔ token
  if (token != Token::tok_number && token != Token::tok_identifier) {
    std::cerr << "Expected expression after ≔ in assignment statement\n";
    return nullptr;
  }

  std::unique_ptr<ExpressionAST> initializer = ParseExpression();
  if (initializer == nullptr) {
    std::cerr
        << "Failed to parse initializer expression in assignment statement\n";
    return nullptr;
  }
  return std::make_unique<AssignmentStatementAST>(*identifier,
                                                  std::move(initializer));
}
} // namespace Babel