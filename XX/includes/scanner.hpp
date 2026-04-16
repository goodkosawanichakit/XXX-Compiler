#pragma once

#include <string>
#include <unordered_map>

namespace XX {

enum class TokenType {
  // Single-character tokens
  PLUS,
  MINUS,
  STAR,
  SLASH,
  SEMICOLON,
  COLON,
  COMMA,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BRACKET,
  RIGHT_BRACKET,
  DOT,
  EQUAL,
  GREATER,
  LESS,

  // LOGICAL
  AND_AND,
  OR_OR,
  NOT,

  // Two-character tokens
  PLUS_EQUAL,
  MINUS_EQUAL,
  SLASH_EQUAL,
  STAR_EQUAL,
  DOT_DOT,
  NOT_EQUAL,
  EQUAL_EQUAL,
  GREATER_EQUAL,
  LESS_EQUAL,
  RETURN_TYPE,

  // Literals
  IDENTIFIER,
  STRING,
  NUMBER_INT,
  NUMBER_FLOAT,

  // Keywords
  KW_INT,
  KW_FLOAT,
  KW_BOOL,
  KW_IF,
  KW_ELSE,
  KW_LOOP,
  KW_FN,
  KW_RETURN,
  KW_TRUE,
  KW_FALSE,

  ERROR,
  TOKEN_EOF
};

struct Token {
  TokenType type;
  // TODO: Change string to String_view to avoid unnecessary allocation
  // Technical Debt my ass
  std::string lexeme;
  int line;
};

class Scanner {
private:
  const std::string &source;
  size_t start = 0;
  size_t current = 0;
  int line = 1;
  static std::unordered_map<std::string, TokenType> reserve_words;

  bool isAtEnd();
  bool match(char c);
  char advance();
  char peek();
  char peekNext();
  void skipWhitespace();
  void comment();
  bool multiLineComment();
  Token digit();
  Token string();
  Token identifier();

public:
  Scanner(const std::string &source);
  Token scanToken();
};

} // namespace XX
