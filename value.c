#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "object.h"
#include "memory.h"
#include "value.h"

void initValueArray(ValueArray *array)
{
	array->values = NULL;
	array->capacity = 0;
	array->count = 0;
}

void writeValueArray(ValueArray *array, Value value)
{
	if (array->capacity < array->count + 1) {
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->values = GROW_ARRAY(Value, array->values, oldCapacity,
					   array->capacity);
	}

	array->values[array->count] = value;
	array->count++;
}

void freeValueArray(ValueArray *array)
{
	FREE_ARRAY(Value, array->values, array->capacity);
	initValueArray(array);
}

SerializedValue *serializeValue(Value value)
{
	size_t serializedSize;
	ObjString *v = AS_STRING(value);

	switch (value.type) {
	case VAL_OBJ: {
		switch (AS_OBJ(value)->type) {
		case OBJ_STRING: {
			serializedSize = sizeof(SerializedValue) + v->length;
			break;
		}
		default:
			return NULL;
			break;
		}
		break;
	}
	default:
		serializedSize = sizeof(SerializedValue);
		break;
	}

	SerializedValue *ser = malloc(serializedSize);
	
	ser->totalSize = serializedSize;

	if (IS_STRING(value)) {
		ser->type = SER_STRING;
		serializeString(v, &(ser->as.string));
	} else {
		ser->type = value.type;
	}

	return ser;
}

void printValue(Value value)
{
	switch (value.type) {
	case VAL_BOOL:
		printf(AS_BOOL(value) ? "true" : "false");
		break;
	case VAL_NIL:
		printf("nil");
		break;
	case VAL_NUMBER:
		printf("%g", AS_NUMBER(value));
		break;
	case VAL_OBJ:
		printObject(value);
		break;
	default:
		break;
	}
}
bool valuesEqual(Value a, Value b)
{
	if (a.type != b.type)
		return false;

	switch (a.type) {
	case VAL_BOOL:
		return AS_BOOL(a) == AS_BOOL(b);
		break;
	case VAL_NIL:
		return true;
	case VAL_NUMBER:
		return AS_NUMBER(a) == AS_NUMBER(b);
	case VAL_OBJ:
		return AS_OBJ(a) == AS_OBJ(b);
	default:
		return false;
	}
}
