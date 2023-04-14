/* this file contains the implementaion of the Config class and its nested class LocationContext */

#include <Config.hpp>

const std::string Config::ServerContext::defaultHostname = "0.0.0.0";
const std::string Config::ServerContext::defaultPort = "80";

Config::Config(const char *fileName) {

	if (!fileName)
		throw std::invalid_argument("file name cannot be NULL");

	setSupportedCGIExtensions();

}

Config::Servers& Config::getServers() { return mServers; }

Config::LocationContext::LocationContext()
	: get(), post(), del(), autoindex() {}

Config::ServerContext::ServerContext()
	: socketID(-1)
	, client_body_size_max() {}

void Config::setSupportedCGIExtensions() {
	
	// defines the supported cgi extensions
	const char extensionsArray[][10] = {".py", ".php"};

	// adds the supported cgi extensions to the member mSupportedCGIExtensions
	for (size_t i = 0; i < sizeof(extensionsArray) / sizeof(extensionsArray[0]); ++i)
		mSupportedCGIExtensions.insert(extensionsArray[i]);

}
