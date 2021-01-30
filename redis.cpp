#include "redis.h"

Redis::Redis() {
    client.instance();
}

Error Redis::connect(String host, int port) {
    IP_Address ip;

	if (host.is_valid_ip_address()) {
		ip = host;
	} else {
		ip = IP::get_singleton()->resolve_hostname(host);
	}

    return client->connect_to_host(ip, port);
}

String convert(Variant value) {
    String cmd;
    switch(value.get_type()) {
        case Variant::STRING: 
        {
            String str = value;
            cmd += "$" + itos(str.length()) + "\r\n" + str.ascii().get_data() + "\r\n";
            break;
        }
        case Variant::INT:
        {
            int i = value;
            cmd += ":" + itos(i) + "\r\n";
            break;
        }
        case Variant::ARRAY:
        {
            Array arr = value;
            cmd += "*" + itos(arr.size()) + "\r\n";

            for(int i = 0; i < arr.size(); i++) {
                cmd += convert(arr.get(i));
            }

            break;
        }
        default:
            ERR_PRINT("Only strings, ints and arrays are allowed");
            break;
    }

    return cmd;
}

Variant Redis::exec(Array command) {
    String cmd = "*";
    cmd += itos(command.size()) + "\r\n";

    for(int i = 0; i < command.size(); i++) {
        auto value = command.get(i);

        cmd += convert(value);
    }

    auto data = cmd.ascii();

    for(int i = 0; i < data.length(); i++) {
        client->put_u8(data.get(i));
    }

    return _read_response();
}

Variant Redis::_read_response() {
    char c = client->get_u8();
    switch(c) {
        case '+': // string
        {
            String res;
            while((c = client->get_u8()) != '\r') {
                res += c;
            }
            client->get_u8();
            return res;
        }
        case '-': // error
        {
            String res;
            while((c = client->get_u8()) != '\r') {
                res += c;
            }
            client->get_u8();
            ERR_PRINT(res);
            return res;
        }
        case ':': // integer
        {
            String res;
            while((c = client->get_u8()) != '\r') {
                res += c;
            }
            client->get_u8();

            return res.to_int();
        }
        case '$': // bulk strings
        {
            String string_length;
            while((c = client->get_u8()) != '\r') {
                string_length += c;
            }
            client->get_u8();

            auto str_len = string_length.to_int();

            String str;
            for(int i = 0; i < str_len; i++) {
                str += client->get_u8();
            }
            client->get_u8();
            client->get_u8();

            return str;
        }
        case '*': // array
        {
            String array_length;
            while((c = client->get_u8()) != '\r') {
                array_length += c;
            }
            client->get_u8();

            auto arr_len = array_length.to_int();
            Array arr;

            for(int i = 0; i < arr_len; i++) {
                arr.append(_read_response());
            }

            return arr;
        }
    }

    return Variant();
}

void Redis::_bind_methods() {
    ClassDB::bind_method(D_METHOD("connect_to_host", "host", "port"), &Redis::connect);
    ClassDB::bind_method(D_METHOD("exec", "command"), &Redis::exec);
}