//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_CLIENT_HPP
#define ROBO_SIMULATOR_CLIENT_HPP

#include "socket.hpp"

class Client : public ClientSocket {
public:

    /*!
     * Creates Client object
     */
    explicit Client();

    /*!
     * Destructor
     */
    ~Client();

    /*!
     * @brief Will try to connect to given address will push information on provided raw pointer
     * @param con  Reference too connection object
     * @return status failed or successfull
     */
    auto establishConnection(Connection &con) -> Response override;

private:
    struct sockaddr_in m_server_addr{};

};

#endif //ROBOSIMULATOR_CLIENT_HPP
