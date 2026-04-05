#include "scanner.hpp"

XX::Scanner::Scanner(const std::string &source) : source(source) {}

bool XX::Scanner::isAtEnd() { return current >= source.length(); }

bool XX::Scanner::match(char c) {
  if (isAtEnd())
    return false;
  if (source[current] != c)
    return false;
  current++;
  return true;
}

char XX::Scanner::advance() { return source[current++]; }

char XX::Scanner::peek() {
  if (isAtEnd())
    return '\0';
  return source[current];
}

void XX::Scanner::skipWhitespace() {
  while (!isAtEnd())
    switch (peek()) {
    case ' ':
    case '\t':
    case '\r':
      advance();
      break;
    case '\n':
      line++;
      advance();
      break;
    default:
      return;
    }
}

XX::Token XX::Scanner::scanToken() {
  skipWhitespace();

  if (isAtEnd())
    return Token{TokenType::TOKEN_EOF, "", line};

  char c = advance();

  switch (c) {
  case '+':
    return Token{TokenType::PLUS, "+", line};
  case '-':
    return Token{TokenType::MINUS, "-", line};
  case '*':
    return Token{TokenType::STAR, "*", line};
  case '/':
    return Token{TokenType::SLASH, "/", line};
  case '(':
    return Token{TokenType::LEFT_PAREN, "(", line};
  case ')':
    return Token{TokenType::RIGHT_PAREN, ")", line};
  case '{':
    return Token{TokenType::LEFT_BRACE, "{", line};
  case '}':
    return Token{TokenType::RIGHT_BRACE, "}", line};
  case ';':
    return Token{TokenType::SEMICOLON, ";", line};
  case ':':
    return Token{TokenType::COLON, ":", line};
  case '.':
    if (match('.'))
      return Token{TokenType::DOT_DOT, "..", line};
    return Token{TokenType::DOT, ".", line};
  case '=':
    if (match('='))
      return Token{TokenType::EQUAL_EQUAL, "==", line};
    return Token{TokenType::EQUAL, "=", line};
  case '!':
    if (match('='))
      return Token{TokenType::NOT_EQUAL, "!=", line};
    return Token{TokenType::NOT, "!", line};
  case '<':
    if (match('='))
      return Token{TokenType::LESS_EQUAL, "<=", line};
    return Token{TokenType::LESS, "<", line};
  case '>':
    if (match('='))
      return Token{TokenType::GREATER_EQUAL, ">=", line};
    return Token{TokenType::GREATER, ">", line};
  }

  return Token{TokenType::TOKEN_EOF, "", line};
}
