#ifndef REDIS_H
#define REDIS_H

#include "core/object.h"
#include "core/reference.h"
#include "core/io/stream_peer_tcp.h"

class Redis : public Object {
    GDCLASS(Redis, Object);

public:
    Redis();

protected:
    static void _bind_methods();

    Error connect(String host, int port);
    Variant exec(Array command);

private:
    Variant _read_response();

private:
    Ref<StreamPeerTCP> client;
};

#endif