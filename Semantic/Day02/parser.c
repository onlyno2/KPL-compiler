/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "debug.h"

Token *currentToken;
Token *lookAhead;

extern Type* intType;
extern Type* charType;
extern SymTab* symtab;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  // TODO: create, enter, and exit program block
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  // create Program Object
  Object *progObject = createProgramObject(currentToken->string);

  eat(SB_SEMICOLON);

  // compile Program block
  enterBlock(progObject->progAttrs->scope);
  compileBlock();
  exitBlock();
  eat(SB_PERIOD);
}

void compileBlock(void) {
  // TODO: create and declare constant objects
  /* 
    syntax: CONST a = 10; a = b;
  */
  Object *constObject;
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    do {
      // read Ident and create object
      eat(TK_IDENT);
      constObject = createConstantObject(currentToken->string);

      eat(SB_EQ);
      constObject->constAttrs->value = compileConstant();
      eat(SB_SEMICOLON); // end of a declare
      declareObject(constObject);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock2();
  } 
  else compileBlock2();
}

void compileBlock2(void) {
  // TODO: create and declare type objects
  /*
    syntax: TYPE T = INTEGER;
  */
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    Object *typeObject = NULL;
    do {
      // read Ident and create object
      eat(TK_IDENT);
      typeObject = createTypeObject(currentToken->string);

      eat(SB_EQ);
      typeObject->typeAttrs->actualType = compileType();
      eat(SB_SEMICOLON); // end of a decleare
      declareObject(typeObject);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  // TODO: create and declare variable objects
  /*
    syntax: VAR a : INTEGER;
  */
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    Object *varObject = NULL;
    do {
      // read Ident and create object
      eat(TK_IDENT);
      varObject = createVariableObject(currentToken->string);

      eat(SB_COLON);
      varObject->varAttrs->type = compileType();
      eat(SB_SEMICOLON); // end of a declare
      declareObject(varObject);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileSubDecls(void) {
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else compileProcDecl();
  }
}

void compileFuncDecl(void) {
  // TODO: create and declare a function object
  /*
    syntax: FUNCTION X() : INTEGER;
  */
  // create FUNCTION object and enter FUNCTION block
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  Object *funcObject = createFunctionObject(currentToken->string);
  enterBlock(funcObject->funcAttrs->scope);

  // add param
  compileParams();
  eat(SB_COLON);

  funcObject->funcAttrs->returnType = compileBasicType();
  eat(SB_SEMICOLON); // end of function signature

  compileBlock(); 
  eat(SB_SEMICOLON); // end of function declare
  exitBlock(); // exit Function block

  declareObject(funcObject);
}

void compileProcDecl(void) {
  // TODO: create and declare a procedure object
  /*  
    syntax: PROCEDURE X();
  */
  // create PROCEDURE object and enter PROCEDURE block
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  Object *procObject = createProcedureObject(currentToken->string);
  enterBlock(procObject->procAttrs->scope);
  // add param
  compileParams();
  eat(SB_SEMICOLON); // end of procedure signature
  compileBlock();
  eat(SB_SEMICOLON); // end of procedure declare
  exitBlock();

  declareObject(procObject);
}

ConstantValue* compileUnsignedConstant(void) {
  // TODO: create and return an unsigned constant value
  ConstantValue* constValue;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    Object *obj = lookupObject(currentToken->string); // check if exist Ident

    if(obj == NULL) { // not found
      error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
    } else if(obj->kind != OBJ_CONSTANT) { // conflict object type
      error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
    }
    
    constValue = duplicateConstantValue(obj->constAttrs->value); // duplicate constantValue
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

ConstantValue* compileConstant(void) {
  // TODO: create and return a constant
  ConstantValue* constValue = NULL;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    constValue = compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    constValue = compileConstant2();
    constValue->intValue = - constValue->intValue; // change sign
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

ConstantValue* compileConstant2(void) {
  // TODO: create and return a constant value
  ConstantValue* constValue = NULL;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    Object *obj = lookupObject(currentToken->string); // check if exist Ident

    if(obj == NULL) { // not found
      error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
    } else if(obj->kind != OBJ_CONSTANT) { // conflict object type
      error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
    }
    
    constValue = duplicateConstantValue(obj->constAttrs->value); // duplicate constantValue
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

Type* compileType(void) {
  // TODO: create and return a type
  Type* type;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER);
    type = makeIntType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    int size = currentToken->value;
    eat(SB_RSEL);
    eat(KW_OF);
    type = makeArrayType(size, compileType());
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    Object* obj = lookupObject(currentToken->string);

    if (obj == NULL) {
      error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
    } else if (obj->kind != OBJ_TYPE) {
      error(ERR_INVALID_TYPE, currentToken->lineNo, currentToken->colNo);
    }

    type = duplicateType(obj->typeAttrs->actualType);
    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

Type* compileBasicType(void) {
  // TODO: create and return a basic type
  Type* type;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER);
    type = makeIntType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON) {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
}

void compileParam(void) {
  // TODO: create and declare a parameter
  Object *paramObject = NULL;
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    // read Ident and create paramObject
    eat(TK_IDENT);
    paramObject = createParameterObject(currentToken->string, PARAM_VALUE, symtab->currentScope->owner);
    eat(SB_COLON);

    paramObject->paramAttrs->type = compileType(); // Neu dung compileBasicType thi k nhan ra duoc type minh dinh nghia
    declareObject(paramObject);
    break;
  case KW_VAR:
    // read Ident and create paramObject
    eat(KW_VAR);
    eat(TK_IDENT);
    paramObject = createParameterObject(currentToken->string, PARAM_REFERENCE, symtab->currentScope->owner);
    eat(SB_COLON);
    
    paramObject->paramAttrs->type = compileType(); // Neu dung compileBasicType thi k nhan ra duoc type minh dinh nghia
    declareObject(paramObject);
    break;
  default:
    error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileStatements(void) {
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileLValue(void) {
  eat(TK_IDENT);
  compileIndexes();
}

void compileAssignSt(void) {
  compileLValue();
  eat(SB_ASSIGN);
  compileExpression();
}

void compileCallSt(void) {
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
}

void compileGroupSt(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void) {
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void) {
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
}

void compileArgument(void) {
  compileExpression();
}

void compileArguments(void) {
  switch (lookAhead->tokenType) {
  case SB_LPAR:
    eat(SB_LPAR);
    compileArgument();

    while (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      compileArgument();
    }

    eat(SB_RPAR);
    break;
    // Check FOLLOW set 
  case SB_TIMES:
  case SB_SLASH:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void) {
  compileExpression();
  switch (lookAhead->tokenType) {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  compileExpression();
}

void compileExpression(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    compileExpression2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileExpression2();
    break;
  default:
    compileExpression2();
  }
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  switch (lookAhead->tokenType) {
  case SB_TIMES:
    eat(SB_TIMES);
    compileFactor();
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    compileFactor();
    compileTerm2();
    break;
    // check the FOLLOW set
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileFactor(void) {
  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    switch (lookAhead->tokenType) {
    case SB_LPAR:
      compileArguments();
      break;
    case SB_LSEL:
      compileIndexes();
      break;
    default:
      break;
    }
    break;
  default:
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileIndexes(void) {
  while (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  initSymTab();

  compileProgram();

  printObject(symtab->program,0);

  cleanSymTab();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
