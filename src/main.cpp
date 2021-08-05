//
// Author: Tassilo Tanneberger on 5/8/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include <spdlog/spdlog.h>
#include "environment/environment.hpp"
#include <windows.h>
#include <string>
#include <iostream>

std::string ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}


int main()
{
	spdlog::set_level(spdlog::level::debug);
	const std::string env_file = "./enviromnent.json";
	std::cout << "my directory is " << ExePath() << "\n";
	Environment env{env_file};
	env.initiate();


	return 0;
}