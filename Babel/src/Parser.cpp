#include "Babel/Parser.h"
#include "Babel/AbstractSyntaxTree.h"
#include "Babel/Token.h"
#include "Babel/TokenData.h"
#include <llvm/ADT/StringRef.h>

#include <iostream>

#include <memory>
#include <string>
#include <utility>
namespace Babel {
Babel::Parser::Parser(std::string *filename)
    : token(llvm::StringRef(), Token::tok_empty) {
  lexer = std::make_unique<Lexer>();
  InitOperatorPrecedence();
  lexer->LoadBuffer(filename);
}

void Parser::InitOperatorPrecedence() {
  operatorPrecedence["≔"] = 2;
  operatorPrecedence["≡"] = 5;
  operatorPrecedence["≠"] = 5;
  operatorPrecedence["≼"] = 7;
  operatorPrecedence["≽"] = 7;
  operatorPrecedence["≺"] = 7;
  operatorPrecedence["≻"] = 7;
  operatorPrecedence["⊕"] = 10;
  operatorPrecedence["⊖"] = 10;
  operatorPrecedence["×"] = 20;
  operatorPrecedence["÷"] = 20;
}

// don't return the token type so we rely on the tokenState member variable to
// determine the current token. This is to avoid ambiguity due to the recursive
// nature of the parsing functions, where each function can consume the token.
void Parser::GetNextToken() { token = lexer->GetNextToken(); }

std::unique_ptr<ProgramAST> Parser::Parse() {
  GetNextToken();
  if (token.GetTokenType() == Token::tok_eof) {
    LogError("No functions found in file.", token.GetTokenLocation());
    return nullptr;
  }
  std::unique_ptr<ProgramAST> program = std::make_unique<ProgramAST>();
  while (token.GetTokenType() != Token::tok_eof) {
    switch (token.GetTokenType()) {
    case Token::tok_eof:
      return nullptr;
    case Token::tok_function:
      program->AddFunction(std::move(ParseFunction()));
      break;
    default:
      LogError("Only functions are supported at the top level.",
               token.GetTokenLocation());
      return nullptr;
    }
  }

  return std::move(program);
}

std::unique_ptr<StatementAST> Parser::ParseStatement() {
  switch (token.GetTokenType()) {
  case Token::tok_if:
    return ParseIfStatement();
  case Token::tok_identifier:
    return ParseIdentifierStatement();
  default:
    return nullptr;
  }
}

std::unique_ptr<StatementAST> Parser::ParseIfStatement() {
  // store the location of the if statement
  TokenLocation location = token.GetTokenLocation();
  GetNextToken(); // consume the if token

  if (token.GetTokenType() != Token::tok_control ||
      token.GetTokenString() != "⟅") {
    LogError("Expected ⟅ after if statement.", token.GetTokenLocation());
    return nullptr;
  }

  GetNextToken();
  if (token.GetTokenType() != Token::tok_number &&
      token.GetTokenType() != Token::tok_identifier) {
    LogError("Expected condition after if statement.",
             token.GetTokenLocation());
    return nullptr;
  }
  // parse condition expression
  std::unique_ptr<ExpressionAST> condition = ParseExpression();

  if (token.GetTokenType() != Token::tok_control ||
      token.GetTokenString() != "⟆") {
    LogError("Expected ⟆ after if statement condition.",
             token.GetTokenLocation());
    return nullptr;
  }

  GetNextToken(); // consume the ⟆
  if (token.GetTokenType() != Token::tok_control ||
      token.GetTokenString() != "꧁") {
    LogError("Expected ꧁ before if statement then branch.",
             token.GetTokenLocation());
    return nullptr;
  }

  std::unique_ptr<StatementAST> thenBranch = ParseStatementBlock();
  if (thenBranch == nullptr) {
    LogError("Failed to parse then branch of if statement.", location);
    return nullptr;
  }

  std::unique_ptr<StatementAST> elseBranch = nullptr;
  if (token.GetTokenType() == Token::tok_else) {
    GetNextToken();
    if (token.GetTokenType() != Token::tok_control ||
        token.GetTokenString() != "꧁") {
      LogError("Expected ꧁ before if statement else branch.",
               token.GetTokenLocation());
      return nullptr;
    }

    elseBranch = ParseStatementBlock();

    if (elseBranch == nullptr) {
      LogError("Failed to parse else branch of if statement.", location);
      return nullptr;
    }
  }

  return std::make_unique<IfStatementAST>(location, std::move(condition),
                                          std::move(thenBranch),
                                          std::move(elseBranch));
}

std::unique_ptr<FunctionAST> Parser::ParseFunction() {
  TokenLocation location = token.GetTokenLocation();
  // Consume the function keyword
  GetNextToken();
  if (token.GetTokenType() != Token::tok_control &&
      token.GetTokenString() != "⟅") {
    LogError("Expected argument list after function declaration.",
             token.GetTokenLocation());
    return nullptr;
  }
  // consume the open parenthesis
  GetNextToken();

  std::vector<std::string> argumentNames;
  bool expectSeparator = false;
  while (token.GetTokenType() != Token::tok_operator &&
         token.GetTokenString() != "⟆") {
    switch (token.GetTokenType()) {
    case Token::tok_control:
      if (!expectSeparator) {
        LogError("Expected Identifier as part of argument list.",
                 token.GetTokenLocation());
        return nullptr;
      }
      if (token.GetTokenString() != "᨞") {
        LogError("Expected separator.", token.GetTokenLocation());
        return nullptr;
      }
      // eat the ᨞
      GetNextToken();
      expectSeparator = false;
      break;
    case Token::tok_identifier:
      if (expectSeparator) {
        LogError("Expected separator after identifier.",
                 token.GetTokenLocation());
        return nullptr;
      }
      argumentNames.push_back(token.GetTokenString());
      GetNextToken();
      expectSeparator = true;
      break;
    default:
      LogError("Invalid argument or separator.", token.GetTokenLocation());
      return nullptr;
    }
  }

  if (token.GetTokenType() != Token::tok_control &&
      token.GetTokenString() != "⟆") {
    LogError("Expected end of argument list.", token.GetTokenLocation());
    return nullptr;
  }
  // consume the close parenthesis
  GetNextToken();

  if (token.GetTokenType() != Token::tok_identifier) {
    LogError("Expected identifier in function declaration.",
             token.GetTokenLocation());
    return nullptr;
  }
  std::string functionName = token.GetTokenString();
  // map 主要的 to main so that the linker knows the entry point
  if (functionName == "主要的") {
    functionName = "main";
  }

  // consume the identifier
  GetNextToken();

  // Create the prototype
  std::unique_ptr<PrototypeAST> prototype =
      std::make_unique<PrototypeAST>(functionName, argumentNames);

  if (token.GetTokenType() != Token::tok_control &&
      token.GetTokenString() != "꧁") {
    LogError("Expected ꧁ at the beginning of the function.",
             token.GetTokenLocation());
    return nullptr;
  }
  GetNextToken(); // consume the ꧁ token

  std::unique_ptr<StatementBlockAST> body =
      std::make_unique<StatementBlockAST>();
  while (!CheckEndOfFunction()) {
    std::unique_ptr<StatementAST> statement = ParseStatement();

    if (statement == nullptr) {
      LogError("Failed to parse statement in function body.", location);
      return nullptr;
    }

    body->AddStatement(std::move(statement));
  }
  return std::make_unique<FunctionAST>(location, std::move(prototype),
                                       std::move(body));
}

std::unique_ptr<StatementAST> Parser::ParseStatementBlock() {
  TokenLocation location = token.GetTokenLocation();
  if (token.GetTokenType() != Token::tok_control &&
      token.GetTokenString() != "꧁") {
    LogError("Expected ꧁ at the beginning of the statement block.",
             token.GetTokenLocation());
    return nullptr;
  }
  GetNextToken(); // consume the ꧁ token

  std::unique_ptr<StatementBlockAST> function =
      std::make_unique<StatementBlockAST>(location);
  while (!CheckEndOfFunction()) {
    std::unique_ptr<StatementAST> statement = ParseStatement();

    if (statement == nullptr) {
      LogError("Failed to parse statement in statement block.", location);
      return nullptr;
    }

    function->AddStatement(std::move(statement));
  }
  return std::move(function);
}

std::unique_ptr<StatementAST> Parser::ParseIdentifierStatement() {
  TokenLocation location = token.GetTokenLocation();
  std::string identifier = token.GetTokenString();
  GetNextToken(); // consume the identifier token

  if (token.GetTokenType() == Token::tok_operator &&
      token.GetTokenString() == "≔") {
    return ParseAssignmentStatement(location, identifier);
  }

  if (token.GetTokenType() == Token::tok_control &&
      token.GetTokenString() == "⟅") {
    return ParseFunctionCall(location, identifier);
  }

  LogError("Unexpected token after identifier. Expected either assignment or "
           "function call.",
           token.GetTokenLocation());
  return nullptr;
}

std::unique_ptr<StatementAST>
Parser::ParseFunctionCall(TokenLocation location, std::string functionName) {
  if (token.GetTokenType() != Token::tok_control ||
      token.GetTokenString() != "⟅") {
    LogError("Expected ⟅ after function call.", token.GetTokenLocation());
    return nullptr;
  }

  GetNextToken(); // consume the ⟅ token

  std::vector<std::unique_ptr<ExpressionAST>> arguments;
  bool expectExpression = true;
  while (token.GetTokenString() != "⟆") {
    if (expectExpression) {
      std::unique_ptr<ExpressionAST> argument = ParseExpression();

      if (argument == nullptr) {
        LogError("Failed to parse argument in function call.",
                 token.GetTokenLocation());
        return nullptr;
      }

      arguments.push_back(std::move(argument));
      expectExpression = false;
      continue;
    }

    if (token.GetTokenString() != "᨞") {
      LogError("Expected ᨞ between function arguments.",
               token.GetTokenLocation());
      return nullptr;
    }
    // consume the ᨞
    GetNextToken();
    expectExpression = true;
  }

  GetNextToken(); // consume the ⟆ token

  if (token.GetTokenType() != Token::tok_control ||
      token.GetTokenString() != "~") {
    LogError("Expected ~ at the end of the function call statement. Multiple "
             "arguments are not supported yet.",
             token.GetTokenLocation());
    return nullptr;
  }

  if (!CheckEndOfStatement()) {
    return nullptr;
  }

  return std::make_unique<FunctionCallStatementAST>(
      location, std::move(functionName), std::move(arguments));
}

std::unique_ptr<StatementAST>
Parser::ParseAssignmentStatement(TokenLocation location,
                                 std::string identifier) {
  if (token.GetTokenType() != Token::tok_operator ||
      token.GetTokenString() != "≔") {
    LogError("Expected ≔ after identifier in assignment statement.",
             token.GetTokenLocation());
    return nullptr;
  }

  GetNextToken(); // consume the ≔ token
  if (token.GetTokenType() != Token::tok_number &&
      token.GetTokenType() != Token::tok_identifier) {
    LogError("Expected expression after ≔ in assignment statement.",
             token.GetTokenLocation());
    return nullptr;
  }

  std::unique_ptr<ExpressionAST> initializer = ParseExpression();
  if (initializer == nullptr) {
    LogError("Failed to parse initializer expression in assignment statement.",
             location);
    return nullptr;
  }
  if (!CheckEndOfStatement()) {
    return nullptr;
  }
  return std::make_unique<AssignmentStatementAST>(
      location, std::move(identifier), std::move(initializer));
}

std::unique_ptr<ExpressionAST> Parser::ParseExpression() {
  auto LeftHandSide = ParsePrimary();
  if (LeftHandSide == nullptr) {
    LogError("Failed to parse expression.", token.GetTokenLocation());
    return nullptr;
  }

  return ParseBinaryOpRHS(0, std::move(LeftHandSide));
}

std::unique_ptr<ExpressionAST>
Parser::ParseBinaryOpRHS(int precedence,
                         std::unique_ptr<ExpressionAST> leftHandSide) {
  // keep going as long as the next token is an operator with higher precedence
  // than the current precedence
  while (true) {
    // if the next token is not an operator, we're done parsing the
    // binary expression
    // this might cause an issue if it's a control character
    if (token.GetTokenType() != Token::tok_operator) {
      return leftHandSide;
    }

    // get the precedence of the current operator token
    std::string operatorStr = token.GetTokenString();
    auto operatorPrecedenceIt = operatorPrecedence.find(operatorStr);
    

    // check if it's a valid operator
    if (operatorPrecedenceIt == operatorPrecedence.end()) {
      LogError(std::string("Invalid operator: ") + operatorStr + ".",
               token.GetTokenLocation());
      return leftHandSide;
    }

    // then check if the operator precedence is high enough to continue parsing
    int tokenPrecedence = operatorPrecedenceIt->second;
    if (tokenPrecedence < precedence) {
      return leftHandSide;
    }

    // consume the operator token after we determine if we can use it here
    // we want the level up to see it if we don't use it for this expression
    GetNextToken(); 

    // now we continue parsing the right hand side. It should be a valid value.
    auto RightHandSide = ParsePrimary();
    if (RightHandSide == nullptr) {
      return nullptr;
    }

    // if the next token isn't an operator, the expression is finished
    if (token.GetTokenType() != Token::tok_operator) {
      return std::make_unique<BinaryExpressionAST>(
          operatorStr, std::move(leftHandSide), std::move(RightHandSide));
    }

    // If the next token after parsing the current operator is another operator,
    // we need to check it's precedence to determine if we keep parsing through
    // the tree
    std::string nextOperatorStr = token.GetTokenString();
    auto nextOperatorPrecedenceIt = operatorPrecedence.find(nextOperatorStr);
    int nextTokenPrecedence = -1;

    if (nextOperatorPrecedenceIt != operatorPrecedence.end()) {
      nextTokenPrecedence = nextOperatorPrecedenceIt->second;
    }

    // if the next token has greater precedence, we give it the current right
    // hand side as it's left hand side, and that becomes the next right hand
    // side. We increase the precedence we pass in to ensure that operators of
    // the same precedence are grouped correctly
    if (nextTokenPrecedence > tokenPrecedence) {
      RightHandSide =
          ParseBinaryOpRHS(tokenPrecedence + 1, std::move(RightHandSide));
      if (RightHandSide == nullptr) {
        return nullptr;
      }
    }

    // now we can combine the left and right hand sides with the current
    // operator
    leftHandSide = std::make_unique<BinaryExpressionAST>(
        operatorStr, std::move(leftHandSide), std::move(RightHandSide));
  }
}

std::unique_ptr<ExpressionAST> Parser::ParsePrimary() {
  switch (token.GetTokenType()) {
  case Token::tok_identifier:
    return ParseIdentifierExpression();
  case Token::tok_number:
    return ParseNumberExpression();
  default:
    return nullptr;
  }
}

std::unique_ptr<ExpressionAST> Parser::ParseIdentifierExpression() {
  TokenLocation location = token.GetTokenLocation();
  std::string identifier = token.GetTokenString();
  GetNextToken(); // consume the identifier token
  return std::make_unique<VariableExpressionAST>(location, identifier);
}

std::unique_ptr<ExpressionAST> Parser::ParseNumberExpression() {
  TokenLocation location = token.GetTokenLocation();

  std::string numberStr = token.GetTokenString();
  GetNextToken(); // consume the number token
  if (numberStr.find('.') != std::string::npos) {
    LogError("Floating point numbers not supported.", token.GetTokenLocation());
    return nullptr;
  }

  return std::make_unique<IntExpressionAST>(location, std::stoi(numberStr));
}

// all statements will end with  ~.
// closing parenthesis (⟆) may need to be added in the future
bool Parser::CheckEndOfStatement() {
  if (token.GetTokenType() != Token::tok_control ||
      token.GetTokenString() != "~") {
    LogError("Expected ~ at the end of the statement.",
             token.GetTokenLocation());
    return false;
  }

  GetNextToken(); // consume the ~ token

  return true;
}

// all statements will end with  ꧂.
// closing parenthesis (⟆) may need to be added in the future
bool Parser::CheckEndOfFunction() {
  if (token.GetTokenType() != Token::tok_control ||
      token.GetTokenString() != "꧂") {
    return false;
  }

  GetNextToken(); // consume the ꧂ token

  return true;
}

void Parser::LogError(const std::string &error, TokenLocation location) {
  std::cerr << "{ Line : " << location.GetLine()
            << " Position : " << location.GetColumn() << " } " << error << '\n';
}
} // namespace Babel