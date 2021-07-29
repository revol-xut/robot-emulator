//
// Author: Tassilo Tanneberger on 23/7/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "environment.hpp"

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
		
		} else if (component_type == "record_server") {
			std::string path = component["path"].asString();
			std::string arguments = component["record_type"].asString();

			// possible types: tcp server, tcp client, udp server, udp client

			

		}

	}
}