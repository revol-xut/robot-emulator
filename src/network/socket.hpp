//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_SOCKET_HPP
#define ROBO_SIMULATOR_SOCKET_HPP

#include <spdlog/spdlog.h>

#include "../type_traits/type_traits.hpp"

#ifdef __unix__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#ifdef _WIN32 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

constexpr int kTimeout = 64;
constexpr unsigned kWaitTime = 1;  // in Milliseconds

/*!
 * @brief Structure in which Connection will be saved.
 */
struct Connection {
    std::string host = "";
    unsigned short port = 0;
};


class SocketInterface : public DataSink, public DataSource {
protected:
    bool m_valid = true; /*! Flag that states if this socket is still writeable or readable. */
    int m_socket = -1; /*! Socket Integer that gets passed to the system call.*/

#ifdef _WIN32
private:
    static bool static_initialized; /*! states if the WSA already got initialized*/
    static void initWSA();
#endif
public:
    SocketInterface();
    explicit SocketInterface(int socket);
    ~SocketInterface();


    auto operator=(const SocketInterface& other_socket)->SocketInterface&;

    /*!
     * @brief Tries to send given reference over given socket
     * @param message reference to RawMessage struct
     * @return status
     */
    auto write(const RawMessage& message)->Response final;

    /*!
     * @brief Tries to send the given string over given socket
     * @param message Message as std::string
     * @return status
     */
    auto write(const std::string& data)->Response final;

    /*!
     * @brief Tries to receive and will return a read data package and if the read was successfull
     */
    auto readMessage() -> std::optional<RawMessage> final;

    /*!
     * @brief Tries to receive and will return a read data package and if the read was successfull
     */
    auto readString() -> std::optional<std::string> final;

    /*!
     * @brief Closes this socket
     */
    void closeSocket();

    /*!
     * @brief Returns if this socket is still usable
     * @return valid
     */
    auto good() const -> bool override;

    /*!
     * @brief Returns this socket
     * @return socket
     */
    auto getSocket() const noexcept -> int;

    /*!
     * @brief Enables usablility of this sockets
     */
    void setValid() noexcept;

    /*!
    * @brief Sets this socket into its invalid state
    */
    void setInvalid() noexcept;
};

struct AcceptedConnection {
    Response state;
    SocketInterface socket;
    std::string source;
};


class ClientSocket : public SocketInterface {
public:
    /*!
     * @brief Will try to connect to given address
     * @param connection this struct includes host and port
     * @return status
     */
    virtual auto establishConnection(Connection& connection) -> Response = 0;

};

class ServerSocket : public SocketInterface {
public:
    /*!
     * @brief Fully virtual method that will set the socket on listening mode
     */
    virtual void listenConnection() = 0;

    /*!
     * @brief Checks if there is a new Connection returns the container.
     * @param timeout_socket is an addition configuration option when timeout_socket is true the socket will timeout
     * if the receiving process takes longer than 5 milliseconds
     * @return if a connection was found and the corresponding socket if a connection was present 
     */
    virtual auto acceptConnection(bool timeout_socket = false) -> AcceptedConnection = 0;
    
    /*!
     * @brief binds this socket to the address struct
     */
    virtual auto bindToAddr() -> Response = 0;

};

#endif //ROBO_SIMULATOR_SOCKET_HPP
