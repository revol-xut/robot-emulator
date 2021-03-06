//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "socket.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>


#if defined(_WIN32) || defined(_WIN64) 
bool BaseSocket::static_initialized = false;

void BaseSocket::initWSA() {
    if (static_initialized) {
        return;
    }

    WSADATA wsa_data;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    if (err != 0) {
        spdlog::critical("WSAStartup failed with error : {0:i}", err);
        throw std::runtime_error("failed in socket creation");
    }
    static_initialized = true;
}
#endif

auto BaseSocket::getLastError() const noexcept -> int {
    int error_number = -1;
#ifdef __unix__
    error_number = errno;
#endif
#if defined(_WIN32) || defined(_WIN64) 
    error_number = WSAGetLastError();
#endif
    return error_number;
}

SocketInterface::SocketInterface() {
#if defined(_WIN32) || defined(_WIN64) 
    BaseSocket::initWSA();
#endif
}

SocketInterface::SocketInterface(int socket): SocketInterface() {
    m_socket = socket;
    if(m_socket < 0){
        m_valid = false;
    }
}

SocketInterface::~SocketInterface() = default;

auto SocketInterface::write(const std::string &message) -> Response {
    if (send(m_socket, message.data(), static_cast<int>(message.size()), 0) < 0) {
        m_valid = false;
        return Response::Failure;
    }
     
    return Response::Success;
}


auto SocketInterface::write(const RawMessage& message) -> Response {
    if (send(m_socket, (char*)message.data.data(), static_cast<int>(message.size), 0) < 0) {
        m_valid = false;
        return Response::Failure;
    }

    return Response::Success;
}


auto SocketInterface::readMessage() ->  std::optional<RawMessage> {
    RawMessage message{};
    int bytes_readen = recv(m_socket, message.data.data(), kBufferSize, 0);
    message.size = std::max(0, bytes_readen);

    // error handling if something goes wrong
    if(bytes_readen < 0){
        spdlog::debug("TCP Socket received error number:" + BaseSocket::getLastError());
        m_valid = false;
        return std::nullopt;
    }

    return message;
}


auto SocketInterface::readString() -> std::optional<std::string> {
    std::string message;
    message.resize(kBufferSize);
    int bytes_readen = recv(m_socket, message.data(), kBufferSize, 0);
    message.resize(std::max(0, bytes_readen));

    // error handling if something goes wrong
    if (bytes_readen < 0) {
        spdlog::debug("TCP Socket received error number:" + BaseSocket::getLastError());
        m_valid = false;
        return std::nullopt;
    }else if(bytes_readen == 0){
        return std::nullopt;
    }
    
    return message;
}

auto operator>>(SocketInterface& socket, std::string& data) -> DataSink& {
    auto read_data = socket.readString();
    data = read_data.value_or("");

    return socket;
}


void SocketInterface::closeSocket() {
    m_valid = false;
#ifdef __unix__
    close(m_socket);
#endif
#ifdef _WIN32
    closesocket(m_socket);
#endif
}


auto SocketInterface::good() const -> bool {
    return m_valid;
}

auto SocketInterface::getSocket() const noexcept -> int {
    return m_socket; 
}

void SocketInterface::setValid() noexcept {
    m_valid = true;
};

void SocketInterface::setInvalid() noexcept {
    m_valid = false;
}

auto SocketInterface::operator=(const SocketInterface& other_sock) -> SocketInterface& {
    m_socket = other_sock.m_socket;
    m_valid = other_sock.m_valid;
    return *this;
}
