#ifndef WHISTLE_SHARE_H
#define WHISTLE_SHARE_H

#include "../logshare/utilities-pp.hpp"
#include "../logshare/nblogio.h"

namespace whistle {

    NBL_MAKE_ENUM_FULL(status, NOT_RUNNING, LISTENING, NOT_LISTENING, HEARD)

    static const int PORT = 30005;

    static inline status connect(status request) {
        nbl::io::client_socket_t socket;
        if ( nbl::io::connect_pipe(socket, PORT) ) {
            return NOT_RUNNING;
        }

        status from;
        if (nbl::io::send_exact(socket, 1, &request, 0)) NBL_ERROR("error sending to whistle");
        if (nbl::io::recv_exact(socket, 1, &from, 0)) NBL_ERROR("error recving from whistle");

        return from;
    }
}

#endif //WHISTLE_SHARE_H