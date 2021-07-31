//
// Author: Tassilo Tanneberger on 19/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_RECORD_SERVER_HPP
#define ROBO_SIMULATOR_RECORD_SERVER_HPP


#include "recorder.hpp"

// For essential networking capabilities
#include "../network/public_server.hpp"
#include "../network/client.hpp"

#include <vector>
#include <chrono>
#include <memory>

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

// This file just contains wrapper for variues use cases

//
// PLaybackClient
// ==============
// Connects to a server socket and starts transmitting its record file
//

class PlaybackClient {
private:
	std::string m_host = "";
	unsigned short m_port = 0;

	Client m_client;

public:
	PlaybackClient() noexcept = default;
	~PlaybackClient();

	auto operator=(PlaybackClient&& rhs) noexcept -> PlaybackClient&;

	auto connect(const std::string& host, unsigned short port ) -> Response;
	void play_file(const std::string& path);
	void play_record(const Recorder& rec);
	void close();
};

//
// PLaybackServer
// ==============
// Waits for connection and plays record file for every connected client
//

class PlaybackServer {
private:
	PublicServer m_server;
	Recorder m_recorder;

	std::string m_file;
	//std::vector<std::thread> m_currently_sending;

public:
	PlaybackServer() noexcept = default;
	PlaybackServer(const PlaybackServer& other_server); // copy constructor
	PlaybackServer(const std::string& ip, unsigned short port);
	~PlaybackServer();

	auto operator=(PlaybackServer&& rhs) noexcept -> PlaybackServer&;

	void listenAndAccept(const std::string& file, double duration = -1.0);
	void close();
};

//
// RecordClient
// ============
// Connects to server socket and creates record file from received data
//


class RecordClient {
private:
	Client m_client;

public:
	RecordClient() noexcept = default;
	~RecordClient();

	auto operator=(RecordClient&& rhs) noexcept -> RecordClient&;

	auto connect(const std::string& host, unsigned short port) -> Response;
	void record(const std::string& file_path, double duration = -1.0);
	void close();
};

//
// RecordServer
// ============
//


class RecordServer {
private:
	std::string m_host;
	unsigned short m_port;

	PublicServer m_server;
	Recorder m_recorder;

public:
	RecordServer() noexcept = default;
	RecordServer(const std::string& ip, unsigned short port);
	~RecordServer();

	auto operator=(RecordServer&& rhs) noexcept -> RecordServer&;

	void listenAndAccept(const std::string& base_path, double duration = -1.0);
	void close();
};

#endif //ROBO_SIMULATOR_RECORD_SERVER_HPP

