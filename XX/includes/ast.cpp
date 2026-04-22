#pragma once
namespace XX::AST {

enum class Kind {
  EXPR,
  BINARY_EXPR,
  INT_LITERAL,
  FLOAT_LITERAL,
  LITERAL_EXPR,
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
};

} // namespace XX::AST
