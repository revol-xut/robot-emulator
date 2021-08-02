//
// Author: Tassilo Tanneberger on 2/8/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "udp_socket.hpp"


SocketUdp::SocketUdp(const std::string& host, unsigned short port) : m_host(host), m_port(port) { //WSA STARTUP?

    auto str_port = std::to_string(port);

    int r = getaddrinfo(host.c_str(), str_port.c_str(), &m_sender_addr, &m_address_info);
    if (r != 0) {
        spdlog::critical("invalid address for creating an udp socket");
    }

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (m_socket < 0) {
        spdlog::critical("cannot create udp socket");
    }

};

auto SocketUdp::readMessage()->std::optional<RawMessage> {
    RawMessage message{};
    

    auto n_bytes_received = recvfrom(m_socket, message.data.data(), kBufferSize, 0, &m_sender_addr, sizeof(m_sender_addr));

    // error handling if something goes wrong
    if (n_bytes_received < 0) {
        spdlog::debug("UDP Socket received error number while receiving:" + errno);
        return std::nullopt;
    }
    else if (n_bytes_received == 0) {
        return std::nullopt;
    }

    return message;
}

auto SocketUdp::readString()->std::optional<std::string> {
    std::string message;
    message.resize(kBufferSize);
    socklen_t size = sizeof(m_sender_addr);
    int n_bytes_received = recvfrom(m_socket, message.data(), kBufferSize, 0, &m_sender_addr, &size);
    message.resize(std::max(0, n_bytes_received));

    // error handling if something goes wrong
    if (n_bytes_received < 0) {
        spdlog::debug("UDP Socket received error number while receiving:" + errno);
        return std::nullopt;
    } else if (n_bytes_received == 0) {
        return std::nullopt;
    }

    return message;
};

auto SocketUdp::write(const RawMessage& message)->Response {
    int n_bytes_transmitted = sendto(m_socket, 
                            message.data.data(), 
                            message.data.size(), 
                            0, 
                            m_address_info.ai_addr, 
                            m_address_info.ai_addrlen);

    if (n_bytes_transmitted < 0) {
        spdlog::debug("UDP Socket received error number while transmitting:" + errno);
        return Response::Failure;
    }
    else if (n_bytes_transmitted < message.size) {
        spdlog::debug("UDP Socket send less than anticipated " 
            + std::to_string(n_bytes_transmitted) + " expected: " 
            + std::to_string(message.size));
        return Response::Failure;
    }
    return Response::Success;
};

auto SocketUdp::write(const std::string& data)->Response {
    int n_bytes_transmitted = sendto(m_socket, data.data(), data.size(), 0, m_address_info.ai_addr, m_address_info.ai_addrlen);

    if (n_bytes_transmitted < 0) {
        spdlog::debug("UDP Socket received error number while transmitting:" + errno);
        return Response::Failure;
    }
    else if (n_bytes_transmitted < data.size()) {
        spdlog::debug("UDP Socket send less than anticipated "
            + std::to_string(n_bytes_transmitted) + " expected: "
            + std::to_string(data.size()));
        return Response::Failure;
    }
    return Response::Success;
};

auto SocketUdp::good() const -> bool {
    return (m_socket > 0);
}