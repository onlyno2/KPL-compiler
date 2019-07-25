/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT; // chi can cai nay vi k can arraySize va elementType do k phai kieu array
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR; // chi can cai nay vi k can arraySize va elementType do k phai kieu array
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY; // kieu du lieu
  type->arraySize = arraySize;  // kich co mang
  type->elementType = elementType; // kieu cua phan tu trong mang
  return type;
}

Type* duplicateType(Type* type) {
  // TODO
  Type* newType = (Type*)malloc(sizeof(Type)); // khoi tao 1 type moi

  newType->typeClass = type->typeClass;
  newType->arraySize = type->arraySize;
  newType->elementType = type->elementType;

  return newType;
}

int compareType(Type* type1, Type* type2) {
  // TODO
  if(type1->typeClass == type2->typeClass) {
    if(type1->typeClass == TP_ARRAY) { // type1->typeClass = type2->typeClass = TP_ARRAY
      if(type1->arraySize != type2->arraySize || compareType(type1->elementType, type2->elementType) == 0) {
        return 0;
      } else {
        return 1; 
      }
    }
  }

  return 0; // type1->typeClass != type2->typeClass
}

void freeType(Type* type) {
  // TODO
  if(type->elementType != NULL) { // if type->typeClass = TP_ARRAY
    freeType(type->elementType);
  }
  free(type);
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  // TODO
  ConstantValue *newIntConstantValue = (ConstantValue*)malloc(sizeof(ConstantValue));

  newIntConstantValue->type = TP_INT;
  newIntConstantValue->intValue = i;

  return newIntConstantValue;
}

ConstantValue* makeCharConstant(char ch) {
  // TODO
  ConstantValue *newCharConstantValue = (ConstantValue*)malloc(sizeof(ConstantValue));

  newCharConstantValue->type = TP_CHAR;
  newCharConstantValue->charValue = ch;

  return newCharConstantValue;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // TODO
  ConstantValue *newConstantValue = (ConstantValue*)malloc(sizeof(ConstantValue));

  newConstantValue->type = v->type;
  if(v->type == TP_CHAR) {
    newConstantValue->type = TP_CHAR;
  } else if(v->type == TP_INT) {
    newConstantValue->type = TP_INT;
  }

  return newConstantValue;
}

/******************* Object utilities ******************************/

Scope* CreateScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* CreateProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = CreateScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* CreateConstantObject(char *name) {
  // TODO
  Object *newConstantObject = (Object*)malloc(sizeof(Object));

  strcpy(newConstantObject->name, name);
  newConstantObject->kind = OBJ_CONSTANT;
  newConstantObject->constAttrs = (ConstantAttributes*)malloc(sizeof(ConstantAttributes));
  newConstantObject->constAttrs->value = (ConstantValue*)malloc(sizeof(ConstantValue));

  return newConstantObject;
}

Object* CreateTypeObject(char *name) {
  // TODO
  Object *newTypeObject = (Object*)malloc(sizeof(Object));

  strcpy(newTypeObject->name, name);
  newTypeObject->kind = OBJ_TYPE;
  newTypeObject->typeAttrs = (TypeAttributes*)malloc(sizeof(TypeAttributes));
  newTypeObject->typeAttrs->actualType = (Type*)malloc(sizeof(Type));

  return newTypeObject;
}

Object* CreateVariableObject(char *name) {
  // TODO
  Object *newVariableObject = (Object*)malloc(sizeof(Object));

  strcpy(newVariableObject->name, name);
  newVariableObject->kind = OBJ_VARIABLE;
  newVariableObject->varAttrs = (VariableAttributes*)malloc(sizeof(VariableAttributes));
  newVariableObject->varAttrs->scope = CreateScope(newVariableObject, symtab->currentScope);
  newVariableObject->varAttrs->type = (Type*)malloc(sizeof(Type));

  return newVariableObject;
}

Object* CreateFunctionObject(char *name) {
  // TODO
  Object *newFunctionObject = (Object*)malloc(sizeof(Object));

  strcpy(newFunctionObject->name, name);
  newFunctionObject->kind = OBJ_FUNCTION;
  newFunctionObject->funcAttrs = (FunctionAttributes*)malloc(sizeof(FunctionAttributes));
  newFunctionObject->funcAttrs->scope = CreateScope(newFunctionObject, symtab->currentScope);
  newFunctionObject->funcAttrs->paramList = (ObjectNode*)malloc(sizeof(ObjectNode));
  newFunctionObject->funcAttrs->returnType = (Type*)malloc(sizeof(Type));

  return newFunctionObject;
}

Object* CreateProcedureObject(char *name) {
  // TODO
  Object *newProcedureObject = (Object*)malloc(sizeof(Object));

  strcpy(newProcedureObject->name, name);
  newProcedureObject->kind = OBJ_PROCEDURE;
  newProcedureObject->procAttrs = (ProcedureAttributes*)malloc(sizeof(ProcedureAttributes));
  newProcedureObject->procAttrs->scope = CreateScope(newProcedureObject, symtab->currentScope);
  newProcedureObject->procAttrs->paramList = (ObjectNode*)malloc(sizeof(ObjectNode));

  return newProcedureObject;
}

Object* CreateParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // TODO
  Object *newParameterObject = (Object*)malloc(sizeof(Object));

  strcpy(newParameterObject->name, name);
  newParameterObject->kind = OBJ_PARAMETER;
  newParameterObject->paramAttrs = (ParameterAttributes*)malloc(sizeof(ParameterAttributes));
  newParameterObject->paramAttrs->kind = kind;
  newParameterObject->paramAttrs->type = (Type*)malloc(sizeof(Type));
  newParameterObject->paramAttrs->function = owner;

  return newParameterObject;
}

void freeObject(Object* obj) {
  // TODO
  if(obj != NULL) {
    switch (obj->kind)
    {
      case OBJ_CONSTANT:
        if(obj->constAttrs->value != NULL) {
          free(obj->constAttrs->value);
        }
        free(obj->constAttrs);
        break;
      case OBJ_TYPE:
        if(obj->typeAttrs->actualType != NULL) {
          freeType(obj->typeAttrs->actualType);
        }
        free(obj->typeAttrs);
        break;
      case OBJ_VARIABLE:
        if(obj->varAttrs->scope != NULL) {
          free(obj->varAttrs->scope);
        }
        if(obj->varAttrs->type != NULL) {
          freeType(obj->varAttrs->type);
        }
        free(obj->varAttrs);
        obj->varAttrs = NULL;
        break;
      case OBJ_PARAMETER:
        if(obj->paramAttrs->type != NULL) {
            freeType(obj->paramAttrs->type);
        }
        if(obj->paramAttrs->function != NULL) {
          free(obj->paramAttrs->function);
        } // k can kind vi kind la int
        free(obj->paramAttrs);

        break;
      case OBJ_FUNCTION:
        if(obj->funcAttrs->paramList != NULL) {
          freeReferenceList(obj->funcAttrs->paramList);
        }
        if(obj->funcAttrs->returnType != NULL) {
          freeType(obj->funcAttrs->returnType);
        }
        if(obj->funcAttrs->scope != NULL) { // scope chua het cac cai ben trong nen chac chi can cai nay thoi la du :)
          freeScope(obj->funcAttrs->scope);
        }
        free(obj->funcAttrs);
        break;
      case OBJ_PROCEDURE:
        if(obj->procAttrs->paramList != NULL) {
          freeReferenceList(obj->procAttrs->paramList);
        } 
        if(obj->procAttrs->scope != NULL) {
          freeScope(obj->procAttrs->scope);
        }
        free(obj->procAttrs);
        break;
      case OBJ_PROGRAM:
        if(obj->progAttrs->scope != NULL) {
          freeScope(obj->progAttrs->scope);
        }
        free(obj->progAttrs);
        break;    
      default:
        break;
    }

    free(obj);
  }
}

void freeScope(Scope* scope) {
  // TODO
  if(scope != NULL) {
    freeObjectList(scope->objList);
    free(scope);
    scope = NULL;
  }
}

void freeObjectList(ObjectNode *objList) {
  // TODO
  if(objList != NULL) {
    freeObject(objList->object);
    freeObjectList(objList->next);
    objList = NULL;
  }
}

void freeReferenceList(ObjectNode *objList) {
  // TODO
  ObjectNode *curr = objList;
  while(objList != NULL) {
    curr = objList;
    objList = objList->next;
    free(curr);
  }
}

void AddObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  // TODO
  ObjectNode *currentObject = objList;
  while(currentObject != NULL) {
    if(strcmp(currentObject->object->name, name) == 0) {
      return currentObject->object;
    }
    currentObject = currentObject->next;
  }
  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = CreateFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEI");
  param = CreateParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEC");
  param = CreateParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITELN");
  AddObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      AddObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      AddObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  AddObject(&(symtab->currentScope->objList), obj);
}


