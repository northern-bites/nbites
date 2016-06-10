#ifndef WHISTLE_SHARE_H
#define WHISTLE_SHARE_H

#include "../logshare/utilities-pp.hpp"
#include "../logshare/nblogio.h"

namespace whistle {

    NBL_MAKE_ENUM_FULL(whistle_status, NOT_RUNNING, LISTENING, NOT_LISTENING, HEARD)
    NBL_MAKE_ENUM_FULL(whistle_request, STOP, QUERY, START);

    static inline const char * string_status(whistle_status status) {
        return whistle_statusStrings[status];
    }

    static inline const char * string_request(whistle_request request) {
        return whistle_requestStrings[request];
    }

    static const int PORT = 30005;

    static inline whistle_status connect(whistle_request request) {
        nbl::io::client_socket_t socket;
        if ( nbl::io::connect_pipe(socket, PORT) ) {
            return NOT_RUNNING;
        }

        whistle_status from;
        if (nbl::io::recv_exact(socket, 1, &from, 0)) NBL_ERROR("error recving from whistle");
        if (nbl::io::send_exact(socket, 1, &request, 0)) NBL_ERROR("error sending to whistle");

        printf("sent %s, got %s\n", string_request(request), string_status(from));

        return from;
    }
}

#endif //WHISTLE_SHARE_H