# godot-redis
This is a godot module to enable communication with a redis in memory database.

This module is tested against godot version 3.2.x.

# Features
- Dead simple redis client
- No external dependencies

# Currently not supported
- Pipelining
- Subscriptions

# Planned features
- Methods per redis command instead only ``exec``
- Subscribe

# Installation
1. Check out this repository under ``modules/`` with the name ``redis``.
2. Compile the engine as normally

# How to use

## Connect
```gdscript
var redis = Redis.new()
if redis.connect_to_host("localhost", 6379) != OK:
    print("Failed to connect to redis server")
```

### Execute commands
```gdscript
var value = redis.exec(["GET", "key"])
print(value)
```
