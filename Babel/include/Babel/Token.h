#ifndef BABEL_TOKEN_H
#define BABEL_TOKEN_H
namespace Babel {
enum class Token {
  tok_eof,
  tok_extern,
  tok_identifier,
  tok_if,
  tok_else,
  tok_for,
  tok_while,
  tok_operator,
  tok_misc
};
}
#endif