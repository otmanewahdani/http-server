/* this file contains the definition of the Config class.
 * This class contains the details of the configuration of the web server.
 * The configuration comes from a config file that's passed as command line arg to the program
 */

#pragma once

#include <string>
#include <map>
#include <set>

class Config {

	public:
		/******* alias types *******/
		typedef std::string Path;
		typedef std::string Extension;
		typedef unsigned short StatusCode;
		typedef unsigned long Size;
		typedef std::map<StatusCode, Path> StatusCodesWithPaths;
		typedef std::map<Extension, Path> CGISystems;

		/******* nested types *******/
		// holds info about a given location
		struct LocationContext {
			Path route;
			// http methods
			bool get;
			bool post;
			bool del;
			// end of http methods
			StatusCodesWithPaths redirections;
			Path root;
			bool autoindex;
			Path defaultFile;
			CGISystems supportedCGIs;
			Path uploadRoute;

			// constructor
			LocationContext();
		};

		/******* alias types *******/
		typedef std::map<Path, LocationContext> LocationsCollection;

		/******* nested types *******/
		// holds information about a given server
		struct ServerContext {
			std::string hostname;
			std::string port;
			std::string server_name;
			StatusCodesWithPaths errorPages;
			Size clientBodySizeMax;
			LocationsCollection locations;
		};

		/******* member functions *******/
		// sets up a configuration from a config file
		Config(const char *fileName);
	
	private:
		std::set<Extension> supportedCGIExtensions;

};
