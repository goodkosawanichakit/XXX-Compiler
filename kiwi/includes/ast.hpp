#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
namespace KIWI::AST {

enum class Kind {
  EXPR,
  // STMT,
  // DECLR,
  BLOCK,
  BINARY_EXPR,
  UNARY_EXPR,
  IDENTIFIER,
  INT_LITERAL,
  FLOAT_LITERAL,
  RETURN_STMT,
  ASSIGN_STMT,
  VAR_DECLR,
  PARAM_DECLR,
  FUNC_DECLR,
  ERROR_STMT,
  ERROR_EXPR,
  ERROR_DECLR
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

// base class of all Node idk it's just that, do I really need to comment?
class Node {
private:
  Kind kind;
  uint32_t offset;
  uint16_t length;

public:
  inline int getOffset() { return offset; }
  inline int getLength() { return length; }
  inline Kind getKind() { return kind; }

  Node(Kind k, uint32_t o, uint16_t l) : kind(k), offset(o), length(l) {}
  virtual ~Node() {}
};

// AKA Module node, File node.
// why I named it Forest? cause it contains an array of pointer to tree.
// many tree = forest
class Forest {
public:
  std::vector<std::unique_ptr<Node>> vec;
};

// Expr class and yeah the name already told it propose.
class Expr : public Node {
public:
  using Node::Node;
};

// it's a statement duh.
class Stmt : public Node {
public:
  using Node::Node;
};

class Declr : public Node {
public:
  using Node::Node;
};

// It's just a Forest node but for a FuncDeclr and if else, loop
class Block : public Node {
public:
  std::vector<std::unique_ptr<Node>> stmts;
  Block(uint32_t o, uint16_t l, std::vector<std::unique_ptr<Node>> b)
      : Node(Kind::BLOCK, o, l), stmts(std::move(b)) {}
};

// yet again the name already told it propose.
// anyway is op as a char is really a good choice?
// answer to question above: op as a char is ass cause I can't directly use
// substr so yeah no more char
// man I'll never leave the project for 1 month ever again, I surely forgot
// everything
class BinaryExpr : public Expr {
private:
  std::string op;
  std::unique_ptr<Expr> LExpr;
  std::unique_ptr<Expr> RExpr;

public:
  inline std::string getOP() { return op; }
  inline Expr *getLExpr() { return LExpr.get(); }
  inline Expr *getRExpr() { return RExpr.get(); }

  BinaryExpr(uint32_t o, uint16_t l, std::string op,
             std::unique_ptr<Expr> lexpr, std::unique_ptr<Expr> rexpr)
      : Expr(Kind::BINARY_EXPR, o, l), op(op), LExpr(std::move(lexpr)),
        RExpr(std::move(rexpr)) {}
};

class UnaryExpr : public Expr {
private:
  std::string op;
  std::unique_ptr<Expr> expr;

public:
  inline std::string getOP() { return op; }
  inline Expr *getExpr() { return expr.get(); }
  UnaryExpr(uint32_t o, uint16_t l, std::string op, std::unique_ptr<Expr> expr)
      : Expr(Kind::UNARY_EXPR, o, l), op(op), expr(std::move(expr)) {}
};

class Identifier : public Expr {
private:
  std::string name;

public:
  inline std::string getName() { return name; }

  Identifier(uint32_t o, uint16_t l, std::string n)
      : Expr(Kind::IDENTIFIER, o, l), name(n) {}
};

// why do I store in int64_t bruh.
// cause it's the maximum that we can use?
// answer to question above: prob yes so we can store any int literal
// anyways to remind future me this is the terminal of BNF ,FloatLiteral too
class IntLiteral : public Expr {
private:
  int64_t value;

public:
  inline int64_t getValue() { return value; }

  IntLiteral(uint32_t o, uint16_t l, int64_t v)
      : Expr(Kind::INT_LITERAL, o, l), value(v) {}
};

class FloatLiteral : public Expr {
private:
  double value;

public:
  inline double getValue() { return value; }

  FloatLiteral(uint32_t o, uint16_t l, double v)
      : Expr(Kind::FLOAT_LITERAL, o, l), value(v) {}
};

class ReturnStmt : public Stmt {
private:
  std::unique_ptr<Expr> retExpr;

public:
  inline Expr *getExpr() { return retExpr.get(); }
  ReturnStmt(uint32_t o, uint16_t l, std::unique_ptr<Expr> e)
      : Stmt(Kind::RETURN_STMT, o, l), retExpr(std::move(e)) {}
};

class AssignStmt : public Stmt {
private:
  std::unique_ptr<Identifier> ident;
  std::unique_ptr<Expr> expr;

public:
  inline Identifier *getIdent() { return ident.get(); }
  inline Expr *getExpr() { return expr.get(); }
  AssignStmt(uint32_t o, uint16_t l, std::unique_ptr<Identifier> i,
             std::unique_ptr<Expr> e)
      : Stmt(Kind::ASSIGN_STMT, o, l), ident(std::move(i)), expr(std::move(e)) {
  }
};

class VarDeclr : public Declr {
private:
  Type type;
  std::unique_ptr<Identifier> ident;
  std::unique_ptr<Expr> initExpr;

public:
  inline Type getType() { return type; }
  inline Identifier *getIdentifier() { return ident.get(); }
  inline Expr *getExpr() { return initExpr.get(); }

  VarDeclr(uint32_t o, uint16_t l, Type t, std::unique_ptr<Identifier> i,
           std::unique_ptr<Expr> init)
      : Declr(Kind::VAR_DECLR, o, l), type(t), ident(std::move(i)),
        initExpr(std::move(init)) {}
};

class ParamDeclr : public Declr {
private:
  Type type;
  std::unique_ptr<Identifier> ident;

public:
  inline Type getType() { return type; }
  inline Identifier *getIdentifier() { return ident.get(); }
  ParamDeclr(uint32_t o, uint16_t l, Type t, std::unique_ptr<Identifier> i)
      : Declr(Kind::PARAM_DECLR, o, l), type(t), ident(std::move(i)) {}
};

class FuncDeclr : public Declr {
private:
  std::unique_ptr<Identifier> name;
  std::vector<std::unique_ptr<Declr>> params;
  Type returnType;
  std::unique_ptr<Block> block;

public:
  inline Identifier *getName() { return name.get(); }
  inline std::vector<std::unique_ptr<Declr>> &getParams() { return params; }
  inline Type getReturnType() { return returnType; }
  inline Block *getBlock() { return block.get(); }

  FuncDeclr(uint32_t o, uint16_t l, std::unique_ptr<Identifier> name,
            std::vector<std::unique_ptr<Declr>> params, Type returnType,
            std::unique_ptr<Block> block)
      : Declr(Kind::FUNC_DECLR, o, l), name(std::move(name)),
        params(std::move(params)), returnType(returnType),
        block(std::move(block)) {}
};

class ErrorStmt : public Stmt {
private:
  std::string msg;

public:
  inline std::string getMessage() { return msg; }
  ErrorStmt(uint32_t o, uint16_t l, std::string msg)
      : Stmt(Kind::ERROR_STMT, o, l), msg(msg) {}
};

class ErrorExpr : public Expr {
private:
  std::string msg;

public:
  inline std::string getMessage() { return msg; }
  ErrorExpr(uint32_t o, uint16_t l, std::string msg)
      : Expr(Kind::ERROR_EXPR, o, l), msg(msg) {}
};

class ErrorDeclr : public Declr {
private:
  std::string msg;

public:
  inline std::string getMessage() { return msg; }
  ErrorDeclr(uint32_t o, uint16_t l, std::string msg)
      : Declr(Kind::ERROR_DECLR, o, l), msg(msg) {}
};

} // namespace KIWI::AST
