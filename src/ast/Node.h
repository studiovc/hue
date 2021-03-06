// Copyright (c) 2012, Rasmus Andersson. All rights reserved. Use of this source
// code is governed by a MIT-style license that can be found in the LICENSE file.

// Base class for all expression nodes.
#ifndef HUE__AST_NODE_H
#define HUE__AST_NODE_H

#include "Type.h"
#include <sstream>
#include <vector>

namespace hue { namespace ast {

inline static void NodeToStringHeader(int level, std::ostream& ss) {
  if (level > 0) {
    ss << std::endl;
    for (int i=level; i--; ) ss << "  ";
  }
}

class Node;
typedef std::vector<Node*> NodeList;

class Node {
public:
  enum NodeTypeID {
    TNode,

    TFunctionType,
    TValue,
    
    // Expressions
    TExpression,
    TFunction,
    TExternalFunction,
    TBlock,
    TSymbol,
    TAssignment,
    TBinaryOp,
    TCall,
    TConditional,
    TIntLiteral,
    TFloatLiteral,
    TBoolLiteral,
    TDataLiteral,
    TTextLiteral,
    TListLiteral,
    TStructure,
    
    _TypeCount
  };
  
  Node(NodeTypeID t = TNode) : type_(t) {}
  virtual ~Node() {}
  
  inline const NodeTypeID& nodeTypeID() const { return type_; }

  inline bool isFunctionType() const { return type_ == TFunctionType; }
  inline bool isValue() const { return type_ == TValue; }

  inline bool isExpression() const { return type_ >= TExpression; }
  inline bool isFunction() const { return type_ == TFunction; }
  inline bool isExternalFunction() const { return type_ == TExternalFunction; }
  inline bool isCall() const { return type_ == TCall; }
  inline bool isBlock() const { return type_ == TBlock; }
  inline bool isSymbol() const { return type_ == TSymbol; }
  inline bool isAssignment() const { return type_ == TAssignment; }

  inline bool isStructure() const { return type_ == TStructure; }

  inline bool isCallable() const { return isFunction() || isExternalFunction(); }
  
  virtual std::string toString(int level = 0) const {
    std::ostringstream ss; NodeToStringHeader(level, ss); ss << "<Node>";
    return ss.str();
  }

  virtual const char* typeName() const {
    switch (type_) {
      case TFunctionType: return "FunctionType";
      case TValue: return "Value";

      // Expressions
      case TExpression: return "Expression";
      case TFunction: return "Function";
      case TExternalFunction: return "ExternalFunction";
      case TBlock: return "Block";
      case TSymbol: return "Symbol";
      case TAssignment: return "Assignment";
      case TBinaryOp: return "BinaryOp";
      case TCall: return "Call";
      case TConditional: return "Conditional";
      case TIntLiteral: return "IntLiteral";
      case TFloatLiteral: return "FloatLiteral";
      case TBoolLiteral: return "BoolLiteral";
      case TDataLiteral: return "DataLiteral";
      case TTextLiteral: return "TextLiteral";
      case TListLiteral: return "ListLiteral";
      case TStructure: return "Structure";

      default: return "?";
    }
  }
  
private:
  const NodeTypeID type_;
};

// Used for function input parameters
class Value : public Node {
public:
  Value(const Type* T) : Node(TValue), type_(T) {}
  inline const Type* resultType() const { return type_; }
protected:
  const Type* type_;
};

}} // namespace hue.ast

inline static std::ostream& operator<< (std::ostream& os, const hue::ast::Node& node) {
  return os << node.toString();
}
inline static std::ostream& operator<< (std::ostream& os, const hue::ast::Node* node) {
  return os << node->toString();
}

#endif // HUE__AST_NODE_H
