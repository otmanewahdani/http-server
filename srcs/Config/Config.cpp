/* this file contains the implementaion of the Config class and its nested class LocationContext */

#include <Config.hpp>

const std::string Config::ServerContext::defaultHostname = "0.0.0.0";
const std::string Config::ServerContext::defaultPort = "8080";

const std::string Config::defaultConfigFileName
	= "./config_files/default_config";

Config::Config(const char *fileName) {

	if (!fileName)
		fileName = defaultConfigFileName.c_str();
	
	std::ifstream configFile(fileName);
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

Config::ConstLocPtr
	Config::ServerContext::getLocation
	(const std::string& path) const {

	for (LocationsCollection::const_iterator loc =
		locations.begin(); loc != locations.end();
		++loc) {
		
		// if path found, returns address
			// of locaction context
		if (loc->first == path)
			return &loc->second;

	}

	return NULL;

}

std::string Config::LocationContext::replaceByRoot
	(const std::string& path) const {
	
	// if no root was set, makes the path starts
		// at the current directory
	const std::string& replacement = root.empty()
		? "./" : root;

	// takes the end position of the location
		// prefix in the path
	const std::string::size_type
		endOfLocPrefixPos = route.size();

	// removes the location prefix and prepends
		// the replacement to it
	return (replacement +
		std::string(path, endOfLocPrefixPos));

}

Config::ConstLocPtr Config::ServerContext::matchLocation
	(const std::string& path) const {
			
	ConstLocPtr matchedLocation = NULL;

	// The search for the matched location will follow this logic:
	// it will start by matching the whole path against all the
		// locations, and if no match is found, the trailing slash will
		// be deleted and the new subpath is stored in subPath and will
		// be matched again, and if not match is found then the chars
		// that precede the next slash are deleted, and new subpath is 
		// tested again. This process will be repeated until a match is
		// found, or no more chars are left in path 
	Path subPath = path;

	// append "/" to check the case of location route ending with "/"
	subPath += "/";

	// checks each prefix subpath within path if it 
		// matches with a location of the server starting 
		// from the most specific one
	size_t pos;
	while (!subPath.empty()) {
		// find the pos of the last "/"
		pos = subPath.rfind("/");

		// search sub path with the / char
		subPath.erase(pos + 1);
		matchedLocation = getLocation(subPath);
		if (matchedLocation)
			break;

		// search sub path without the / char
		subPath.erase(pos);
		matchedLocation = getLocation(subPath);
		if (matchedLocation)
			break;
	}

	return matchedLocation;

}

void Config::initSupportedCGIExtensions() {
	
	// defines the supported cgi extensions
	const char extensionsArray[][10] = {".pl", ".php"};

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

Config::ConstServerRef Config::getServerRef
	(Socket socketID) const {

	Servers::const_iterator server;
	// iterates until a server with socketID is found
	for (server = mServers.begin();
		server != mServers.end(); ++server)
		if (server->socketID == socketID)
			break;

	// not found
	if (server == mServers.end()) {
		std::string error = "couldn't find a server "
			"that has this socket id: ";
		error += std::to_string(socketID);
		throw std::invalid_argument(error);
	}

	return *server;

}

void Config::print() {

	int serverNum = 1;
	// traverses server's elements
	for (Servers::const_iterator server = mServers.begin();
			server != mServers.end(); ++server) {

		std::cout << "SERVER " << serverNum++ << "\n";
		printServer(*server, 1);
		std::cout << '\n';

	}

}

void Config::printServer(const ServerContext& server, int indent) {

	const std::string indentStr(indent, '\t');

	std::cout << indentStr << "SERVER_NAME: '"
		<< server.server_name << "'\n";

	std::cout << indentStr << "LISTEN: "
		<< server.hostname << ':' << server.port << '\n';

	std::cout << indentStr << "SOCKET_ID: "
		<< server.socketID << "\n";

	std::cout << indentStr << "CLIENT_BODY_SIZE_MAX: "
		<< server.clientBodySizeMax << '\n';
	
	std::cout << indentStr << "ERROR_PAGES\n";
	printMap(server.errorPages, indent + 1);

	// taverses location's elements
	for (LocationsCollection::const_iterator
		it = server.locations.begin();
		it != server.locations.end(); ++it) {

		std::cout << indentStr << "LOCATION "
			<< it->first << '\n';
		printLocation(it->second, indent + 1);
		std::cout << '\n';

	}

}

void Config::printLocation(const LocationContext& location, int indent) {

	const std::string indentStr(indent, '\t');
	
	std::cout << indentStr << "ALLOW_METHODS\n";
	if (location.get)
		std::cout << indentStr + '\t' << "GET\n";
	if (location.post)
		std::cout << indentStr + '\t' << "POST\n";
	if (location.del)
		std::cout << indentStr + '\t' << "DELETE\n";

	std::cout << indentStr << "REDIRECTION\n";
	std::cout << indentStr + '\t' << location.redirection.first
		<< " -> " << location.redirection.second << '\n';

	std::cout << indentStr << "ROOT: '" << location.root << "'\n";

	std::cout << indentStr << "AUTOINDEX: "
		<< (location.autoindex ? "ON\n" : "OFF\n");

	std::cout << indentStr << "DEFAULT: '"
		<< location.defaultFile << "'\n";

	std::cout << indentStr << "CGI\n";
	printMap(location.supportedCGIs, indent + 1);

	std::cout << indentStr << "UPLOAD: '"
		<< location.uploadRoute << "'\n";

}

template <class Map>
void Config::printMap(const Map& elems, int indent) {

	const std::string indentStr(indent, '\t');

	for (typename Map::const_iterator it = elems.begin();
		it != elems.end(); ++it) {
		std::cout << indentStr << it->first << " -> ";
		std::cout << it->second << '\n';
	}

}
