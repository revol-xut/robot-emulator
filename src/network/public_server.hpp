//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//


#ifndef ROBO_SIMULATOR_PUBLIC_SERVER_HPP
#define ROBO_SIMULATOR_PUBLIC_SERVER_HPP

#include "socket.hpp"

constexpr int kConnectionPubCache = 10;
constexpr char kDefaultHost[] = "127.0.0.1";
constexpr unsigned short kDefaultPort = 8422;

class PublicServer : public ServerSocket {
protected:
    struct sockaddr_in m_address{};

    std::string m_host = std::string(kDefaultHost);
    unsigned short m_port = kDefaultPort;

    /*!
     * @brief Bind Socket to the defined values
     * @return status if this operation was successfull or not
     */
    auto bindToAddr() -> Response override;

public:
    /*!
     * @brief Uses default values from config ('host', 'port') to create the exposed server socket
     */
    PublicServer();

    /*!
     * @brief Uses given values to create the exposed server socket
     * @param ip Ipv4 Address
     * @param port Port
     */
    explicit PublicServer(const std::string &ip, unsigned short port);

    /*!
     * @brief Default destructor
     */
    ~PublicServer();

    /*!
     * @brief creates address struct and tries to bind
     */
    virtual auto configure()->Response;

    /*!
     * @brief Listens for incoming connections and accepts them.
     * @return Returns a tuple the first element determins if a connection could
     * be established the second is the socket
     */
    auto acceptConnection(bool timeout) -> AcceptedConnection  override;

    /*!
     * @brief let the socket listen for incoming connections
     */
    void listenConnection() override;

};


#endif //ROBO_SIMULATOR_PUBLIC_SERVER_HPP
