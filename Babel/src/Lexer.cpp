#include "Babel/Lexer.h"
#include "Babel/Token.h"
#include <iostream>
#include <string>
namespace Babel {
Babel::Token Lexer::GetNextToken() {
	if(currentLine.empty()){
		std::getline(std::cin, currentLine);
	}
	const char *currentCharacterPtr = currentLine.c_str();
}

std::vector<char> *Lexer::GetNextCharacter(){
	std::vector<char> nextCharacter;
}
} // namespace Babel