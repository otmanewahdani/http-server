/* this file contains the implementaion of the Config class and its nested class LocationContext */

#include <Config.hpp>

Config::LocationContext::LocationContext() : get(), post(), del(), autoindex() {}

Config::Config(const char *fileName) {
	
	// defines the supported cgi extensions
	const char extensionsArray[][10] = {".py", ".php"};

	// adds the supported cgi extensions to the member supportedCGIExtensions
	for (size_t i = 0; i < sizeof(extensionsArray) / sizeof(extensionsArray[0]); ++i)
		supportedCGIExtensions.insert(extensionsArray[i]);

}
