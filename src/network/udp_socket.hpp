//
// Author: Tassilo Tanneberger on 2/8/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_UDP_SOCKET_HPP
#define ROBO_SIMULATOR_UDP_SOCKET_HPP

#include "../type_traits/type_traits.hpp"
#include "socket.hpp"
#include <iostream>
#include <string>

class SocketUdp : public SuperSocket {
protected:
	int m_socket = -1;

	std::string m_host = "";
	unsigned short m_port = 0;

	//struct sockaddr_in m_addr {};
	struct sockaddr_in m_receiver_addr {}; // when transmitting where it goes
	struct sockaddr_in m_this_sock_addr {}; // describing this socket
	struct sockaddr_in m_temp_recv_addr {}; // used for recv to save the address of the latest transmitter
public:

	SocketUdp();
	explicit SocketUdp(const Connection& this_addr);
	explicit SocketUdp(const Connection& this_addr, const Connection& peer_addr);
	~SocketUdp() noexcept = default;

	/*!
	 * @brief Binds current to socket to address defined by m_host and m_port
	 */
	auto bindSocket()->Response;
	/*!
	 * @brief Defines target at which address all data that is written is send to 
	 */
	void setTarget(const Connection& peer_addr);

	auto readMessage()->std::optional<RawMessage> override;
	auto readString()->std::optional<std::string> override;

	auto write(const RawMessage& message)->Response override;
	auto write(const std::string& data)->Response override;

	auto good() const -> bool override;
};


#endif //ROBO_SIMULATOR_UDP_SOCKET_HPP