#ifndef LOX_FUNCTION
#define LOX_FUNCTION

struct Value* function_call(struct Environment* env, struct Value* callee,
                            struct Value* arguments);

#endif