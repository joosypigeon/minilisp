#ifndef EVAL_H
#define EVAL_H  

#include "object.h"
#include "env.h"


Object *safe_eval(Env *local, Object *expr);

#endif // EVAL_H