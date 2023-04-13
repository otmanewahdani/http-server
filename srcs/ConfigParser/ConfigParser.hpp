/* this file includes the definition of ConfigParser class which represents the complete logic
 * 	that parses the configuration file (given as an input stream) of the web server
 */

#pragma once

#include <Config.hpp>
#include <istream>

class ConfigParser {
	
	public:
		/******* public member functions *******/
		ConfigParser(std::istream& input, Config& config);
	
	private:
		/******* private member functions *******/
		Config& mConfig;

};
