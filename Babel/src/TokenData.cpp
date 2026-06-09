#include "Babel/TokenData.h"
#include "Babel/Token.h"
#include <llvm-20/llvm/ADT/StringRef.h>
namespace Babel {

TokenData::TokenData(llvm::StringRef tokenString, Token tokenType, int line, int column)
    : tokenString(tokenString), tokenType(tokenType), line(line), column(column) {
}

Token TokenData::GetTokenType(){
	return tokenType;
}

std::string TokenData::GetTokenString(){
	return tokenString.str();
}
} // namespace Babel