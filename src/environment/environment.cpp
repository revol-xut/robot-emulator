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

Environment::~Environment() {
	for (std::thread& thread : m_process_threads) {
		thread.join();
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
			

			auto lambda = [path, arguments]() {
				spdlog::debug("Starting Executable {} {}", path, arguments);
				system((path + " " + arguments).c_str());
			};

			
			m_process_threads.emplace_back(lambda);
			spdlog::debug("EXEC Thread started {} {}", path, arguments);

		} else if (component_type == "playback_server") {
			std::cout << component << std::endl;
			std::string rec_type = component["record_type"].asString();
			std::string path = component["record_file"].asString();
			std::string host = component["host"].asString();
			unsigned short port = static_cast<unsigned short>(component["port"].asInt());
			// possible types: tcp server, tcp client, udp server, udp client

			std::function<void(void)> exec;

			if (rec_type == "TCP_SERVER") {
				std::cout << host << "|" << port << "|" << path << std::endl;
				
				exec = [host, port, path] () { //&server is maybe dangerous
					spdlog::debug("Starting PlaybackServer on {}:{}", host, port);
					PlaybackServer server{ host, port };
					server.listenAndAccept(path, -1);
				};

				m_process_threads.emplace_back(exec);


			}else if (rec_type == "TCP_CLIENT") {
				

				exec = [host, port, path]() {
					spdlog::debug("Starting PlaybackClient connecting to {}:{}", host, port);
					PlaybackClient client;
					client.connect(host, port);

					client.play_file(path);
				};

				spdlog::debug("Starting PlaybackClient connecting to {}:{}", host, port);
				m_process_threads.emplace_back(exec);
			}

		}

	}
}