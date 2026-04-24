#include "scanner.hpp"
#include <cstdint>
#include <sys/types.h>

std::unordered_map<std::string, XX::TokenType> XX::Scanner::reserve_words = {
    {"int8", TokenType::KW_INT8},       {"int16", TokenType::KW_INT16},
    {"int32", TokenType::KW_INT32},     {"int64", TokenType::KW_INT64},
    {"float8", TokenType::KW_FLOAT8},   {"float16", TokenType::KW_FLOAT16},
    {"float32", TokenType::KW_FLOAT32}, {"float64", TokenType::KW_FLOAT64},
    {"char", TokenType::KW_CHAR},       {"string", TokenType::KW_STRING},
    {"bool", TokenType::KW_BOOL},       {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},       {"loop", TokenType::KW_LOOP},
    {"fn", TokenType::KW_FN},           {"return", TokenType::KW_RETURN},
    {"true", TokenType::KW_TRUE},       {"false", TokenType::KW_FALSE}};

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
      // line++;
      lineOffset.push_back(current);
      advance();
      break;
    default:
      return;
    }
}

void XX::Scanner::comment() {
  while (peek() != '\n' && !isAtEnd())
    advance();
}

bool XX::Scanner::multiLineComment() {
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
      // line++;
      lineOffset.push_back(current);
      break;
    }
    if (count == 0)
      return true;
  }
  return false;
}

XX::Token XX::Scanner::string() {
  while (peek() != '"' && !isAtEnd())
    advance();

  if (isAtEnd())
    return Token{TokenType::ERROR, (uint32_t)start,
                 (uint16_t)(current - start)};

  advance();
  return Token{TokenType::STRING, (uint32_t)start, (uint16_t)(current - start)};
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

  return (is_float) ? Token{TokenType::NUMBER_FLOAT, (uint32_t)start,
                            (uint16_t)(current - start)}
                    : Token{TokenType::NUMBER_INT, (uint32_t)start,
                            (uint16_t)(current - start)};
}

XX::Token XX::Scanner::identifier() {
  while (std::isalnum(peek()) || peek() == '_')
    advance();

  std::string lexeme = source.substr(start, current - start);
  auto it = reserve_words.find(lexeme);

  if (it != reserve_words.end()) {
    return Token{it->second, (uint32_t)start, (uint16_t)(current - start)};
  } else {
    return Token{TokenType::IDENTIFIER, (uint32_t)start,
                 (uint16_t)(current - start)};
  }
}

XX::Token XX::Scanner::scanToken() {
  skipWhitespace();

  start = current;

  if (isAtEnd())
    return Token{TokenType::TOKEN_EOF, (uint32_t)start, 1};

  char c = advance();

  switch (c) {
  case '+':
    if (match('='))
      return Token{TokenType::PLUS_EQUAL, (uint32_t)start, 2};
    return Token{TokenType::PLUS, (uint32_t)start, 1};
  case '-':
    if (match('='))
      return Token{TokenType::MINUS_EQUAL, (uint32_t)start, 2};
    if (match('>'))
      return Token{TokenType::RETURN_TYPE, (uint32_t)start, 2};
    return Token{TokenType::MINUS, (uint32_t)start, 1};
  case '*':
    if (match('='))
      return Token{TokenType::STAR_EQUAL, (uint32_t)start, 2};
    return Token{TokenType::STAR, (uint32_t)start, 1};
  case '/':
    if (match('/')) {
      comment();
      return scanToken();
    } else if (match('*')) {
      if (!multiLineComment())
        return Token{TokenType::ERROR, (uint32_t)start,
                     (uint16_t)(current - start)};
      return scanToken();
    } else if (match('=')) {
      return Token{TokenType::SLASH_EQUAL, (uint32_t)start, 2};
    }
    return Token{TokenType::SLASH, (uint32_t)start, 1};
  case '(':
    return Token{TokenType::LEFT_PAREN, (uint32_t)start, 1};
  case ')':
    return Token{TokenType::RIGHT_PAREN, (uint32_t)start, 1};
  case '{':
    return Token{TokenType::LEFT_BRACE, (uint32_t)start, 1};
  case '}':
    return Token{TokenType::RIGHT_BRACE, (uint32_t)start, 1};
  case '[':
    return Token{TokenType::LEFT_BRACKET, (uint32_t)start, 1};
  case ']':
    return Token{TokenType::RIGHT_BRACKET, (uint32_t)start, 1};
  case ',':
    return Token{TokenType::COMMA, (uint32_t)start, 1};
  case ';':
    return Token{TokenType::SEMICOLON, (uint32_t)start, 1};
  case ':':
    return Token{TokenType::COLON, (uint32_t)start, 1};
  case '.':
    if (match('.'))
      return Token{TokenType::DOT_DOT, (uint32_t)start, 2};
    return Token{TokenType::DOT, (uint32_t)start, 1};
  case '=':
    if (match('='))
      return Token{TokenType::EQUAL_EQUAL, (uint32_t)start, 2};
    return Token{TokenType::EQUAL, (uint32_t)start, 1};
  case '!':
    if (match('='))
      return Token{TokenType::NOT_EQUAL, (uint32_t)start, 2};
    return Token{TokenType::NOT, (uint32_t)start, 1};
  case '<':
    if (match('='))
      return Token{TokenType::LESS_EQUAL, (uint32_t)start, 2};
    return Token{TokenType::LESS, (uint32_t)start, 1};
  case '>':
    if (match('='))
      return Token{TokenType::GREATER_EQUAL, (uint32_t)start, 2};
    return Token{TokenType::GREATER, (uint32_t)start, 1};
  case '&':
    if (!match('&'))
      return Token{TokenType::ERROR, (uint32_t)start,
                   (uint16_t)(current - start)};
    return Token{TokenType::AND_AND, (uint32_t)start, 2};
  case '|':
    if (!match('|'))
      return Token{TokenType::ERROR, (uint32_t)start,
                   (uint16_t)(current - start)};
    return Token{TokenType::OR_OR, (uint32_t)start, 2};
  case '"':
    return string();
  default:
    if (std::isdigit(c))
      return digit();
    else if (std::isalpha(c) || c == '_') {
      return identifier();
    }
    return Token{TokenType::ERROR, (uint32_t)start,
                 (uint16_t)(current - start)};
  }

  return Token{TokenType::TOKEN_EOF, (uint32_t)start, 1};
}
