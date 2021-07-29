//
// Author: Tassilo Tanneberger on 23/7/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_ENVIRONMENT_HPP
#define ROBO_SIMULATOR_ENVIRONMENT_HPP

#include <json/json.h>

class Environment {
private:
	std::string m_config_file;
	Json m_config;

public:
	Environment() noexcept = default;
	Environment(const std::string& file);
	~Environment() noexcept = default;


	void initiate();


};


#endif //ROBO_SIMULATOR_ENVIRONMENT_HPP