/* this file includes the definition of ConfigParser class which represents the complete logic
 * 	that parses the configuration file (given as an input stream) of the web server
 */

#pragma once

#include <Config.hpp>
#include <istream>

class ConfigParser {
	
	ConfigParser(std::basic_istream& input, Config& config);

};