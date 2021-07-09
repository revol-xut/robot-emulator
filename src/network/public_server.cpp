//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "public_server.hpp"


PublicServer::PublicServer() {
    //configure();
}

PublicServer::PublicServer(const std::string &ip, unsigned short port) : m_host(ip), m_port(port) {
    configure();
};


PublicServer::~PublicServer() {
    //closeSocket();
};

auto PublicServer::configure() -> Response {
#ifdef __WIN32
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        spdlog::critical("WSAStartup failed with error : {0:i}", err);
        return Response::Failure;
    }
#endif

    if ((m_socket = (int)socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        spdlog::critical("Public SocketInterface could not be created");
        return Response::Failure;
    }

    // Configuring address struct 
    m_address.sin_family = AF_INET;
    inet_pton(AF_INET, m_host.c_str(), &m_address.sin_addr);
    m_address.sin_port = htons(m_port);

    // binding address to socket
    if (bindToAddr() == Response::Success) {
        spdlog::debug("Public Socket bind was successful");
        return Response::Success;
    } else {
        spdlog::critical("Bind to Address was not successful");
        return Response::Failure;
    }
}


auto PublicServer::bindToAddr() -> Response {
    const int kTimeoutMilisec = 10;
    
#ifdef __WIN32
    const bool kOpt = true;
#endif
#ifdef __unix__
    const int kOpt = 1;
#endif
    
    if (setsockopt(m_socket, 
            SOL_SOCKET,
            SO_REUSEADDR, 
            (char*)&kOpt,
            sizeof(kOpt))) {
        spdlog::critical("setsockopt failed");
        return Response::Failure;
    }

    if (bind(m_socket, (struct sockaddr *) &m_address, //NOLINT C-Cast is necessary
             sizeof(m_address)) < 0) {
        spdlog::critical("public server socket bind failed");
        return Response::Failure;
    }
    return Response::Success;
}

void PublicServer::listenConnection() {
    if (listen(m_socket, kConnectionPubCache) < 0) {
        spdlog::critical("Listening on public Socket failed");
    }
    //ULONG NonBlock;
    //if (ioctlsocket(m_socket, FIONBIO, &NonBlock) == SOCKET_ERROR){
    //    std::cerr << "ioctlsocket() failed" << std::endl;
    //}

}

auto PublicServer::acceptConnection(bool timeout) -> AcceptedConnection {
    char str[INET_ADDRSTRLEN];
    int new_socket = -1;
    const auto addr_length = sizeof(m_address);
    AcceptedConnection return_value{};

    if ((new_socket = accept(
            m_socket, (struct sockaddr *) &m_address, (socklen_t *) &addr_length)) < 0) { //NOLINT C-Cast is necessary
        spdlog::critical("Accept on public Socket failed");
        m_valid = false;

        return_value.state = Response::Failure;
        return_value.socket = SocketInterface{ -1 };

        return return_value;
    }
    inet_ntop(AF_INET, &(m_address), static_cast<char *>(str), INET_ADDRSTRLEN);

    if (timeout) {
        spdlog::critical("Creating Timeout socket");
        constexpr unsigned int kTimeOutMilis = 5;
        uint32_t timeout = kTimeOutMilis * 100;
        setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    }

    return_value.state = Response::Success;
    return_value.source = std::string(str);
    return_value.socket = SocketInterface{ new_socket };

    return return_value;
}

