//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "client.hpp"

Client::Client() {
#if defined(_WIN32) || defined(_WIN64)
    BaseSocket::initWSA();
#endif

    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        spdlog::critical("could not create client socket error: {} sock: {}", BaseSocket::getLastError(), m_socket);

        throw std::runtime_error("failed in socket creation");
    }
    m_server_addr = {};
}

Client::~Client() = default;


auto Client::establishConnection(Connection &con) -> Response {
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(con.port);

    if (inet_pton(AF_INET, con.host.c_str(), &m_server_addr.sin_addr) <= 0) {
        spdlog::critical("Invalid address_ / Address not unknown");

        return Response::Failure;
    }

    if (connect(
          m_socket,
          (struct sockaddr *)&m_server_addr,  // NOLINT this cast is necessary
          sizeof(m_server_addr)) < 0) {
        spdlog::critical("Connection to other server failed");
        return Response::Failure;
    }
    return Response::Success;
}

