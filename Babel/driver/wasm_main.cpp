#include <emscripten/bind.h>
#include "Babel/Lexer.h"
#include "Babel/Parser.h"
#include "Babel/Interpreter.h"
#include <string>

std::string run_code(std::string source) {
    Lexer lexer(source);
    Parser parser(lexer);
    auto ast = parser.parseProgram();
    if (parser.hasErrors())
        return parser.getErrors();
    Interpreter interp;
    return interp.run(*ast);
}

EMSCRIPTEN_BINDINGS(mylang) {
    emscripten::function("run_code", &run_code);
}