//
// Author: Tassilo Tanneberger on 23/7/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "environment.hpp"
#include "../recorder/record_server.hpp"

#include <spdlog/spdlog.h>
#include <fstream>

Environment::Environment(const std::string& config_file) {
	m_config_file = config_file;
	std::ifstream config_doc(m_config_file, std::ifstream::binary);

	Json::CharReaderBuilder rbuilder;
	std::string errs;
	bool parsingSuccessful = Json::parseFromStream(rbuilder, config_doc, &m_config, &errs);
	if (!parsingSuccessful){
		spdlog::critical("Failed to parse configuration\n" + errs );
		return;
	}
}


void Environment::initiate() {
	std::string  main_executable = m_config["main"].asString();
	const Json::Value other_components = m_config["components"];

	for (Json::Value component : other_components) {
		std::string component_type = component["type"].asString();
		
		if(component_type == "executable"){
			std::string path = component["path"].asString();
			std::string arguments = component["args"].asString();
			
			auto lambda = [path, arguments] {
				system((path + " " + arguments + "\0").data());
			};

			m_process_threads.emplace_back(lambda);

		} else if (component_type == "record_server") {
			std::string rec_type = component["record_type"].asString();
			std::string path = component["record_file"].asString();
			std::string host = component["host"].asString();
			unsigned short port = static_cast<unsigned short>(component["port"].asInt());
			// possible types: tcp server, tcp client, udp server, udp client

			std::function<void(void)> exec;

			if (rec_type == "TCP_SERVER") {
				PlaybackServer server{host, port};
				
				exec = [&server, path] { //&server is maybe dangerous
					server.listenAndAccept(path, -1);
				};

			}else if (rec_type == "TCP_CLIENT") {
				PlaybackClient client;
				client.connect(host, port);

				exec = [&client, path] { //&client is maybe dangerous
					client.play_file(path);
				};
			}

		}

	}
}