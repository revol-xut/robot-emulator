//
// Author: Tassilo Tanneberger on 23/7/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_COMPONENT_HPP
#define ROBO_SIMULATOR_COMPONENT_HPP

#include <json/json.h>
#include <thread>

class Component {
private:
	std::thread m_process_thread;

public:

	Component() noexcept = default;
	Component(const Json& config);
	~Component() noexcept = default;

	void run();

};


#endif //ROBO_SIMULATOR_COMPONENT_HPP
