//
// Author: Tassilo Tanneberger on 2/8/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "udp_socket.hpp"

constexpr int kTimeoutErrorCode = 10060;

SocketUdp::SocketUdp() {
#if defined(_WIN32) || defined(_WIN64)
    BaseSocket::initWSA();
#endif
}

SocketUdp::SocketUdp(const Connection& this_addr) : SocketUdp() { //WSA STARTUP?
    // cannot combine member initializer and the call of the base constrctor
    m_host = this_addr.host;
    m_port = this_addr.port;
    spdlog::debug("This Socket :: host: {} port: {}", m_host, m_port);

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (m_socket < 0) {
        spdlog::critical("cannot create udp socket");
    }

    static int timeout = 100;

    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        spdlog::critical("setsockopt failed for setting the timeout");
    };

    bindSocket(); // otherwise this socket would be wild (wild socket)
};

SocketUdp::SocketUdp(const Connection& this_addr, const Connection& peer_addr) : SocketUdp(this_addr) {
    setTarget(peer_addr);
}


void SocketUdp::setTarget(const Connection& peer_addr) {
    spdlog::debug("Target Socket :: host: {} port: {}", peer_addr.host, peer_addr.port);

    struct hostent* c_host;
    c_host = gethostbyname(peer_addr.host.c_str());
    if (c_host == nullptr) {
        spdlog::critical("cannot resolve address");
        return;
    }

    // address of peer 
    m_receiver_addr.sin_family = c_host->h_addrtype;
    memcpy((char*)&m_receiver_addr.sin_addr.s_addr,
        c_host->h_addr_list[0], c_host->h_length);
    m_receiver_addr.sin_port = htons(peer_addr.port);
}


auto SocketUdp::readMessage()->std::optional<RawMessage> {
    RawMessage message{};
    
    int size = sizeof(m_temp_recv_addr);
    int n_bytes_received = recvfrom(m_socket, message.data.data(), kBufferSize, 0, (sockaddr*)&m_temp_recv_addr, &size);

    // error handling if something goes wrong
    if (n_bytes_received < 0) {
        spdlog::debug("UDP Socket received error number while receiving: {}", this->getLastError());
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
    socklen_t size = sizeof(m_temp_recv_addr);
    int n_bytes_received = recvfrom(m_socket, message.data(), kBufferSize, 0, (sockaddr*)&m_temp_recv_addr, &size);
    message.resize(std::max(0, n_bytes_received));

    // error handling if something goes wrong
    if (n_bytes_received < 0) {
        int error = this->getLastError();
        if (error != kTimeoutErrorCode) { // windows code for timeout
            spdlog::debug("UDP Socket received error number while receiving: {}", error);
        }
        
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
                            (struct sockaddr*)&m_receiver_addr,
                            sizeof(m_receiver_addr));

    if (n_bytes_transmitted < 0) {
        spdlog::debug("UDP Socket received error number while transmitting: {}", this->getLastError());
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
    int n_bytes_transmitted = sendto(m_socket,
        data.data(),
        data.size(),
        0,
        (struct sockaddr*)&m_receiver_addr,
        (int)sizeof(m_receiver_addr));

    if (n_bytes_transmitted < 0) {
        spdlog::debug("UDP Socket received error number while transmitting: {}", this->getLastError());
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
};


auto SocketUdp::bindSocket() -> Response {
    // Configuring address struct 
    m_this_sock_addr.sin_family = AF_INET;
    if (m_host != "0.0.0.0") {
        m_this_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }else {
        inet_pton(AF_INET, this->m_host.c_str(), &m_this_sock_addr.sin_addr);
    }
    m_this_sock_addr.sin_port = htons(this->m_port);


#ifdef _WIN32
    const bool optval = true;
#endif
#ifdef __unix__
    const int optval = 1;
#endif

    int res = setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)(&optval), sizeof(optval));

    if (res < 0) {
        spdlog::debug("UDP Socket setsockopt failed: {}", this->getLastError());
        return Response::Failure;
    };

    res = bind(this->m_socket, (struct sockaddr*)&m_this_sock_addr, sizeof(m_this_sock_addr));

    if (res < 0) {
        spdlog::debug("UDP Socket bind failed: {}", this->getLastError());
        return Response::Failure;
    };

    return Response::Success;
};