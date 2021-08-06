//
// Author: Tassilo Tanneberger on 11/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_SIGNAL_HANDER_HPP
#define ROBO_SIMULATOR_SIGNAL_HANDER_HPP

#include "recorder/recorder.hpp"

#include <csignal>
#include <stdlib.h>
#include <signal.h>
#include <iostream>

void signalHandlerSigTerm(int signum) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	std::cout << "Terminating Recorder ... ";
	Recorder::m_forcefull_termination = true;

	exit(signum);
}

#endif //ROBO_SIMULATOR_SIGNAL_HANDER_HPP
