/* This file contains the definition of the ServerManager class
 * This class manages all the operations of the webserver
 *  It listens for incoming connections
 *  It manages existing clients connections and creates new clients
 *  It forwards clients' requests to the modules that handle requests
 *  It also forwards responses coming from
 *  	other modules to the corresponding clients
 */

#pragma once

#include <Config.hpp>
#include <sys/errno.h>

class ServerManager {

	public:
		/******* public alias types *******/
		typedef Config::Servers Servers;

		/******* public member functions *******/
		// takes a configuration file path
			// and passes it to Config
		// and calls Network::initServersSockets()
		ServerManager(const char* configFileName);

		// starts the operations of the server
			// (handling clients, requests, etc..)
		void start();

		void printConfig();
	
	private:
		/******* private member objects *******/
		Config mConfig;

		/******* private member functions *******/

};
