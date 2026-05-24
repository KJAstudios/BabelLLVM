#ifndef BABEL_LEXER_H
#define BABEL_LEXER_H
#include "Token.h"
#include <llvm-18/llvm/Support/MemoryBuffer.h>
#include <llvm-18/llvm/ADT/StringRef.h>
#include <memory>
#include <string>

namespace Babel
{
  class Lexer
  {
  private:
    // core input buffer
    std::unique_ptr<llvm::MemoryBuffer> rawMemoryBuffer;
    llvm::StringRef stringCodeBuffer;

    // string storage for current token types
    llvm::StringRef identifierStr;
    llvm::StringRef operatorStr;
    llvm::StringRef controlCharacter;
    llvm::StringRef numberStr;

    // helper control variables
    unsigned offset;

  public:
    Babel::Token GetNextToken();
    void LoadBuffer(std::string *filename = nullptr);
    // value/string getters for the current token, used by the parser
    llvm::StringRef *GetOperatorStr() { return &operatorStr; }
    llvm::StringRef *GetControlCharacter() { return &controlCharacter; }
    llvm::StringRef *GetIdentifierStr() { return &identifierStr; }
    llvm::StringRef *GetNumberStr() { return &numberStr; }

  private:
    // core functions for the token parsing loop
    void SkipComment();
    Babel::Token LexNumberToken();
    Babel::Token LexWhitespaceTerminatedToken(unsigned charSize);
    Babel::Token LexControlCharTerminatedToken(unsigned charSize,
                                               llvm::StringRef nextChar);
    Babel::Token LexOperatorTerminatedToken(unsigned charSize,
                                            llvm::StringRef nextChar);
    Babel::Token GetTokIdentifierOrKeyword(llvm::StringRef identifier);

    // lexing helper functions
    unsigned int GetCharSize(char pointer);
    bool IsControlCharacter(llvm::StringRef character);
    bool IsOperatorCharacter(llvm::StringRef character);
    bool IsWhitespaceCharacter(char character);
  };

} // namespace Babel
#endif