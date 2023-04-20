/* This file contains the implementation of the Log class */

#include <Log.hpp>

// log file creation
std::ofstream Log::mLogfile("./log_files/basic_logfile",
	std::ofstream::out | std::ofstream::trunc);