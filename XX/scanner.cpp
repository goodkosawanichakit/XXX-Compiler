#include "scanner.hpp"
#include <cctype>

std::unordered_map<std::string, XX::TokenType> XX::Scanner::reserve_word = {
    {"int", XX::TokenType::KW_INT},   {"float", XX::TokenType::KW_FLOAT},
    {"bool", XX::TokenType::KW_BOOL}, {"if", XX::TokenType::KW_IF},
    {"else", XX::TokenType::KW_ELSE}, {"for", XX::TokenType::KW_LOOP},
    {"in", XX::TokenType::KW_IN},     {"step", XX::TokenType::KW_STEP},
    {"fn", XX::TokenType::KW_FN},     {"return", XX::TokenType::KW_RETURN},
    {"true", XX::TokenType::KW_TRUE}, {"false", XX::TokenType::KW_FALSE}};

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

char XX::Scanner::peekNext() {
  if (current + 1 >= source.length())
    return '\0';
  return source[current + 1];
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

inline void XX::Scanner::comment() {
  while (peek() != '\n' && !isAtEnd())
    advance();
}

// TODO: return TekenType::ERROR when block comment is unterminated
// mcomment() is void, needs signature change to bool or Token
void XX::Scanner::mcomment() {
  int count = 1;
  while (!isAtEnd()) {
    char c = advance();
    switch (c) {
    case '/':
      if (match('*'))
        count++;
      break;
    case '*':
      if (match('/'))
        count--;
      break;
    case '\n':
      line++;
      break;
    }
    if (count == 0)
      return;
  }
}

XX::Token XX::Scanner::string() {
  while (peek() != '"' && !isAtEnd())
    advance();

  if (isAtEnd())
    return Token{TokenType::ERROR, "Unterminated string.", line};

  advance();
  return Token{TokenType::STRING, source.substr(start + 1, current - start - 2),
               line};
}

XX::Token XX::Scanner::digit() {
  bool is_float = false;

  while (isdigit(peek()) && !isAtEnd())
    advance();

  if (peek() == '.' && isdigit(peekNext())) {
    is_float = true;

    advance();

    while (isdigit(peek()) && !isAtEnd())
      advance();
  }

  return (is_float) ? Token{TokenType::NUMBER_FLOAT,
                            source.substr(start, current - start), line}
                    : Token{TokenType::NUMBER_INT,
                            source.substr(start, current - start), line};
}

XX::Token XX::Scanner::identifier() {
  while (std::isalnum(peek()) || peek() == '_')
    advance();

  std::string lexeme = source.substr(start, current - start);
  auto it = reserve_word.find(lexeme);

  if (it != reserve_word.end()) {
    return Token{it->second, lexeme, line};
  } else {
    return Token{TokenType::IDENTIFIER, lexeme, line};
  }
}

XX::Token XX::Scanner::scanToken() {
  skipWhitespace();

  start = current;

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
    if (match('/')) {
      comment();
      return scanToken();
    } else if (match('*')) {
      mcomment();
      return scanToken();
    }
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
  case '"':
    return string();
  default:
    if (std::isdigit(c))
      return digit();
    else if (std::isalpha(c) || c == '_') {
      return identifier();
    }
    return Token{TokenType::ERROR, "Unexpected character.", line};
  }

  return Token{TokenType::TOKEN_EOF, "", line};
}
