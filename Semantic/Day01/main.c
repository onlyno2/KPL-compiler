/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "symtab.h"
#include "debug.h"

extern SymTab* symtab;
/******************************************************************/

int main() {
  Object* obj;

  initSymTab();

  obj = CreateProgramObject("PRG");
  enterBlock(obj->progAttrs->scope);  

  obj = CreateConstantObject("c1");
  obj->constAttrs->value = makeIntConstant(10);
  declareObject(obj);

  obj = CreateConstantObject("c2");
  obj->constAttrs->value = makeCharConstant('a');
  declareObject(obj);

  obj = CreateTypeObject("t1");
  obj->typeAttrs->actualType = makeArrayType(10,makeIntType());
  declareObject(obj);

  obj = CreateVariableObject("v1");
  obj->varAttrs->type = makeIntType();
  declareObject(obj);

  obj = CreateVariableObject("v2");
  obj->varAttrs->type = makeArrayType(10,makeArrayType(10,makeIntType()));
  declareObject(obj);

  obj = CreateFunctionObject("f");
  obj->funcAttrs->returnType = makeIntType();
  declareObject(obj);
  
  enterBlock(obj->funcAttrs->scope);
 
    obj = CreateParameterObject("p1", PARAM_VALUE, symtab->currentScope->owner);
    obj->paramAttrs->type = makeIntType();
    declareObject(obj);

    obj = CreateParameterObject("p2", PARAM_REFERENCE, symtab->currentScope->owner);
    obj->paramAttrs->type = makeCharType();
    declareObject(obj);

    obj = CreateConstantObject("Max");
    obj->constAttrs->value = makeIntConstant(100);
    declareObject(obj);

    Object *obj2 = CreateTypeObject("Vector");
    obj2->typeAttrs->actualType = makeArrayType(obj->constAttrs->value->intValue, makeIntType());
    declareObject(obj2);

    Object *obj3 = CreateVariableObject("X");
    obj3->varAttrs->type = obj2->typeAttrs->actualType;
    declareObject(obj3);
  exitBlock();

  obj = CreateProcedureObject("p");
  declareObject(obj);
  
  enterBlock(obj->procAttrs->scope);
 
    obj = CreateParameterObject("v1", PARAM_VALUE, symtab->currentScope->owner);
    obj->paramAttrs->type = makeIntType();
    declareObject(obj);

    obj = CreateConstantObject("c1");
    obj->constAttrs->value = makeCharConstant('a');
    declareObject(obj);
    
    obj = CreateConstantObject("c3");
    obj->constAttrs->value = makeIntConstant(10);
    declareObject(obj);

    obj = CreateTypeObject("t1");
    obj->typeAttrs->actualType = makeIntType();
    declareObject(obj);

    obj = CreateTypeObject("t2");
    obj->typeAttrs->actualType = makeArrayType(10,makeIntType());
    declareObject(obj);

    obj = CreateVariableObject("v2");
    obj->varAttrs->type = makeArrayType(10,makeIntType());
    declareObject(obj);

    obj = CreateVariableObject("v3");
    obj->varAttrs->type = makeCharType();
    declareObject(obj);

  exitBlock();

  exitBlock();
  printObject(symtab->program,0);
  cleanSymTab();
    
  return 0;
}
