#pragma once
#include <cstdint>
namespace XX::AST {

enum class Kind {
  EXPR,
  BINARY_EXPR,
  INT_LITERAL,
  FLOAT_LITERAL,
};

enum class Type {
  INT8,
  INT16,
  INT32,
  INT64,
  FLOAT8,
  FLOAT16,
  FLOAT32,
  FLOAT64
};

class Node {
private:
  Kind kind;
  int line;
  int column;

public:
  inline int getLine() { return line; }
  inline int getCol() { return column; }
  inline Kind getKind() { return kind; }

  Node(Kind k, int l, int c) : kind(k), line(l), column(c) {}
  ~Node() {}
};

class Expr : public Node {
public:
  using Node::Node;
  ~Expr() {}
};

class BinaryExpr : public Expr {
private:
  char op;
  Expr *LExpr;
  Expr *RExpr;

public:
  inline char getOP() { return op; }
  inline Expr *getLExpr() { return LExpr; }
  inline Expr *getRExpr() { return RExpr; }

  BinaryExpr(int l, int c, char op, Expr *lexpr, Expr *rexpr)
      : Expr(Kind::BINARY_EXPR, l, c), op(op), LExpr(lexpr), RExpr(rexpr) {}

  ~BinaryExpr() {}
};

class IntLiteral : public Expr {
private:
  Type type;
  int64_t value;

public:
  inline Type getType() { return type; }
  inline int64_t getValue() { return value; }

  IntLiteral(int l, int c, Type t, int64_t v)
      : Expr(Kind::INT_LITERAL, l, c), type(t), value(v) {}

  ~IntLiteral() {}
};

class FloatLiteral : public Expr {
private:
  Type type;
  double value;

public:
  inline Type getType() { return type; }
  inline double getValue() { return value; }

  FloatLiteral(int l, int c, Type t, double v)
      : Expr(Kind::FLOAT_LITERAL, l, c), type(t), value(v) {}

  ~FloatLiteral() {}
};

} // namespace XX::AST
