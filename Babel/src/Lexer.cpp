#include "Babel/Lexer.h"
#include "Babel/Token.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <llvm-18/llvm/ADT/StringRef.h>
#include <llvm-18/llvm/Support/MemoryBuffer.h>
#include <memory>
#include <string>
#include <system_error>
namespace Babel {
void Lexer::LoadBuffer(std::string *filename) {
  if (filename != nullptr) {
    TryLoadCodeIntoBuffer(llvm::MemoryBuffer::getFile(*filename));
  }

  TryLoadCodeIntoBuffer(llvm::MemoryBuffer::getSTDIN());
}

void Lexer::TryLoadCodeIntoBuffer(
    const llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> memoryBuffer) {
  if (!memoryBuffer) {
    std::error_code error = memoryBuffer.getError();
    std::cerr << error;
    return;
  }

  stringCodeBuffer = memoryBuffer.get()->getBuffer();
}

Babel::Token Lexer::GetNextToken() {
  if (stringCodeBuffer.empty()) {
    std::cerr << "Buffer is empty\n";
    return Token::tok_eof;
  }

  unsigned offset = 0;

  bool isTokenEnd = false;

  while (!isTokenEnd && offset < stringCodeBuffer.size()) {
    unsigned charSize = GetCharSize(stringCodeBuffer[offset]);
    // if the char size is 1, we run our checks against ASCII characters
    if (charSize == 1) {
      bool isWhitespaceCharacter =
          IsWhitespaceCharacter(stringCodeBuffer[offset]);
      if (offset == 0) {
        if (isWhitespaceCharacter) {
          stringCodeBuffer = stringCodeBuffer.drop_front(1);
          return GetNextToken();
        }
      }

      if (isWhitespaceCharacter) {
        llvm::StringRef token =
            llvm::StringRef(stringCodeBuffer.data(), offset);
        stringCodeBuffer = stringCodeBuffer.drop_front(offset + charSize);
        return GetTokIdentifierOrKeyword(token);
      }
    }

    llvm::StringRef nextChar =
        llvm::StringRef(stringCodeBuffer.data() + offset, charSize);

    if(IsControlCharacter(nextChar)){
      // if the first character is a control character, it's a control token
      if(offset == 0){
        controlCharacter = nextChar;
        stringCodeBuffer = stringCodeBuffer.drop_front(charSize);
        return Token::tok_control;
      }

      // else it's an identifier found before the control character
      llvm::StringRef token =
          llvm::StringRef(stringCodeBuffer.data(), offset);
      stringCodeBuffer = stringCodeBuffer.drop_front(offset + charSize);
      return GetTokIdentifierOrKeyword(token);
    }

    if (IsOperatorCharacter(nextChar)) {
      // if the first character is an operator, it's an operator token
      if(offset == 0){
        operatorStr = nextChar;
        stringCodeBuffer = stringCodeBuffer.drop_front(charSize);
        return Token::tok_operator;
      }

      // else it's an identifier found before the operator
      llvm::StringRef token = llvm::StringRef(stringCodeBuffer.data(), offset);
      stringCodeBuffer = stringCodeBuffer.drop_front(offset + charSize);
      return GetTokIdentifierOrKeyword(token);
    }

    // move to the next character
    offset += charSize;
  }

  // if we reach the end of the buffer, we're at the end of the file/input
  std::cerr << "End of file reached\n";
  return Token::tok_eof;
}

Babel::Token Lexer::GetTokIdentifierOrKeyword(llvm::StringRef identifier) {

  if (identifier.str() == "əgər") {
    return Token::tok_if;
  }
  if (identifier.str() == "それ以外") {
    return Token::tok_else;
  }
  if (identifier.str() == "tisk") {
    return Token::tok_print;
  }
  if (identifier.str() == "ለ") {
    return Token::tok_for;
  }
  if (identifier.str() == "yekjimar") {
    return Token::tok_int;
  }
  if (identifier.str() == "kaksinkertainen") {
    return Token::tok_double;
  }


  identifierStr = identifier;
  return Token::tok_identifier;
}

// Get the size of the unicode character at the current pointer
unsigned int Lexer::GetCharSize(const char pointer) {
  auto value = static_cast<uint8_t>(pointer);
  if (value < 0x80) {
    return 1;
  }
  if (value < 0xE0) {
    return 2;
  }
  if (value < 0xF0) {
    return 3;
  }
  return 4;
}

bool Lexer::IsControlCharacter(llvm::StringRef character) {
  return character.str() == "~" || character.str() == "꧁" ||
         character.str() == "꧂" || character.str() == "⟅" ||
         character.str() == "⟆";
}

bool Lexer::IsWhitespaceCharacter(char character) {
  switch (character) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return true;
  default:
    return false;
  }
}

bool Lexer::IsOperatorCharacter(llvm::StringRef character) {
  if (character.str() == "⊕") {
    return true;
  }
  if (character.str() == "⊖") {
    return true;
  }
  if (character.str() == "×") {
    return true;
  }
  if (character.str() == "÷") {
    return true;
  }
  if (character.str() == "≔") {
    return true;
  }

  return false;
}
} // namespace Babel