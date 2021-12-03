#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "chunk.h"
#include "value.h"

typedef enum {
	OBJ_STRING,
	OBJ_UPVALUE,
	OBJ_FUNCTION,
	OBJ_CLOSURE,
	OBJ_NATIVE,
	OBJ_PROCESS
} ObjType;

struct Obj {
	ObjType type;
	bool isMarked;
	struct Obj *next;
};

typedef struct {
	Obj obj;
	int arity;
	int upvalueCount;
	Chunk chunk;
	ObjString *name;
} ObjFunction;

typedef Value (*NativeFn)(int argCount, Value *args);

typedef struct {
	Obj obj;
	int ppid;
	int childPid;
	int readPipeFd;
} ObjProcess;

typedef struct {
	Obj obj;
	NativeFn function;
} ObjNative;

struct ObjString {
	Obj obj;
	int length;
	char *chars;
	uint32_t hash;
};

typedef struct ObjUpvalue {
	Obj obj;
	Value *location;
	Value closed;
	struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
	Obj obj;
	ObjFunction *function;
	ObjUpvalue **upvalues;
	int upvalueCount;
} ObjClosure;

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))

#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))

#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value))->function)

#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))

#define IS_PROCESS(value) isObjType(value, OBJ_PROCESS)
#define AS_PROCESS(value) ((ObjProcess *)AS_OBJ(value))

#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

ObjClosure *newClosure(ObjFunction *function);
ObjFunction *newFunction();
ObjNative *newNative(NativeFn function);
ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
ObjUpvalue *newUpvalue(Value *slot);
ObjProcess *newProcess(int childPid, int pipefd);
void serializeString(ObjString *string, SerializedObjString *serialized);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type)
{
	return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif
