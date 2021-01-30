#include "register_types.h"
#include "core/class_db.h"

#include "redis.h"

void register_redis_types() {
    ClassDB::register_class<Redis>();
}

void unregister_redis_types() {
}
