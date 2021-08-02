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

class SocketUdp : public DataSink, public DataSource {
private:
	int m_socket = -1;

	std::string m_host = "";
	unsigned short m_port = 0;

	//struct sockaddr_in m_addr {};
	struct addrinfo m_address_info;
	struct sockaddr_in m_sender_addr;
public:

	SocketUdp() noexcept = default;
	SocketUdp(const std::string& host, unsigned short port);
	~SocketUdp() noexcept = default;

	auto readMessage()->std::optional<RawMessage> override;
	auto readString()->std::optional<std::string> override;

	auto write(const RawMessage& message)->Response override;
	auto write(const std::string& data)->Response override;

	auto good() const -> bool override;
};

#endif //ROBO_SIMULATOR_UDP_SOCKET_HPP