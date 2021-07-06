//
// Author: Tassilo Tanneberger on 19/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "record_server.hpp"

#include <limits>

//
// This file are primarly premade usecases for the recorder class
//

//
// PlaybackClient
// 

auto PlaybackClient::connect(const std::string& host, unsigned short port) -> Response {
	Connection connection{
		host,
		port
	};

	auto response = m_client.establishConnection(connection);
	return response;
};

PlaybackClient::~PlaybackClient() {
	close();
}

void PlaybackClient::play_file(const std::string& path) {
	if (!m_client.good()) {
		return;
	}

	Recorder rec{};
	rec.transmittFile(m_client, path);
}

void PlaybackClient::play_record(const Recorder& rec) {
	rec.transmittObj(m_client);
}

void PlaybackClient::close() {
	m_client.closeSocket();
}

auto PlaybackClient::operator=(PlaybackClient&& rhs) -> PlaybackClient& {
	rhs.m_client = m_client;
	rhs.m_host = m_host;
	rhs.m_port = m_port;
	return rhs;
}



//
// PlaybackServer
// 

PlaybackServer::PlaybackServer(const std::string& ip, unsigned short port) {
	m_server = PublicServer{ ip, port };
	Recorder m_recorder;
}

PlaybackServer::~PlaybackServer() {
	close();
}

void PlaybackServer::listenAndAccept(const std::string& file, double duration) {
	// enters listing mode
	m_server.listenConnection();
	bool endless = false;

	if (duration >= 0) {
		duration *= pow(10, 7);
	} else {
		endless = true;
	}

	auto start = std::chrono::system_clock::now();

	// accepts connection and forks process
	// Terminate and (Time_reaced or Endless)
	while (!Recorder::m_forcefull_termination 
		&& ((std::chrono::system_clock::now() - start).count() < duration || endless) 
		&& m_server.good()) {

		auto possible_connection = m_server.acceptConnection(false);

		if (possible_connection.state == Response::Success) {

			m_recorder.transmittFile(possible_connection.socket, file);

			//m_currently_sending.emplace_back(&Recorder::transmittObj, &m_recorder, possible_connection.socket);
		}
	}
}


auto PlaybackServer::operator=(PlaybackServer&& rhs)->PlaybackServer& {
	rhs.m_file = m_file;
	rhs.m_server = m_server;
	return rhs;
}

void PlaybackServer::close() {
	m_server.closeSocket();
}

//
// RecordClient
//


auto RecordClient::connect(const std::string& host, unsigned short port) -> Response {
	Connection connection{
		host,
		port
	};

	return m_client.establishConnection(connection);
};

RecordClient::~RecordClient() {
	close();
}

void RecordClient::record(const std::string& file_path, double duration) {
	Recorder recorder{};

	recorder.receive(m_client, file_path, Duration{ duration });
}

auto RecordClient::operator=(RecordClient&& rhs) ->RecordClient& {
	rhs.m_client = m_client;
	return rhs;
}

void RecordClient::close() {
	m_client.closeSocket();
}
	

//
// RecordServer
//


RecordServer::RecordServer(const std::string& ip, unsigned short port) : m_host(ip), m_port(port) {
	m_server = PublicServer{ ip, port };
};

RecordServer::~RecordServer(){
	close();
};

void RecordServer::listenAndAccept(const std::string& base_path, double duration){
	m_server.listenConnection();
	bool endless = false;

	if (duration >= 0) {
		duration *= pow(10, 7);
	}
	else {
		duration = DBL_MAX;
		endless = true;
	}

	auto start = std::chrono::system_clock::now();

	// accepts connection and forks process
	// Terminate and (Time_reaced or Endless)
	while (!Recorder::m_forcefull_termination
		&& ((std::chrono::system_clock::now() - start).count() < duration || endless)
		&& m_server.good()) {

		auto possible_connection = m_server.acceptConnection(false);

		if (possible_connection.state == Response::Success) {

			std::string file_name = base_path + "record_file_" + possible_connection.source
				+ "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) 
				+ ".rd";

			auto chrono_duration = Duration(duration - (std::chrono::system_clock::now() - start).count());
			m_recorder.receive(possible_connection.socket, file_name, chrono_duration);
		}
	}
}

auto RecordServer::operator=(RecordServer&& rhs)->RecordServer& {
	rhs.m_host = m_host;
	rhs.m_port = m_port;
	rhs.m_server = m_server;
	return rhs;
}


void RecordServer::close() {
	m_server.closeSocket();
}