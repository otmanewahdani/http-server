/* this file contains the implementaion of the Config class and its nested class LocationContext */

#include <Config.hpp>

const std::string Config::ServerContext::defaultHostname = "0.0.0.0";
const std::string Config::ServerContext::defaultPort = "80";

Config::Config(const char *fileName) {

	if (!fileName)
		throw std::invalid_argument("file name cannot be NULL");
	
	std::fstream configFile(fileName);
	if (!configFile) {
		std::string error = "config file '";
		error += fileName;
		error += "' couldn't be opened for reading";
		throw std::invalid_argument(error);
	}

	initSupportedCGIExtensions();

	// passes the this Config object to ConfigParser so that the parsing
		// result is stored in it
	ConfigParser configParser(configFile, *this);

	configFile.close();

}

Config::Servers& Config::getServers() { return mServers; }

Config::LocationContext::LocationContext()
	: get(), post(), del(), autoindex() {}

Config::ServerContext::ServerContext()
	: socketID(-1)
	, clientBodySizeMax() {}

void Config::initSupportedCGIExtensions() {
	
	// defines the supported cgi extensions
	const char extensionsArray[][10] = {".py", ".php"};

	// adds the supported cgi extensions to the member mSupportedCGIExtensions
	for (size_t i = 0; i < sizeof(extensionsArray) / sizeof(extensionsArray[0]); ++i)
		mSupportedCGIExtensions.insert(extensionsArray[i]);

}

bool Config::isCGIExtensionSupported
	(const Extension& extension) {

		// found if it doesn't equal past-the-end iterator
		if (mSupportedCGIExtensions.find(extension) !=
			mSupportedCGIExtensions.end())
				return true;

		return false;

}
