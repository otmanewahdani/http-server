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

class ServerManager {

	public:
		/******* public member functions *******/
		// takes a configuration file path
			// and passes it to Config
		// and calls initSockets
		ServerManager(const char* configFileName);

		// starts the operations of the server
			// (handling clients, requests, etc..)
		void start();
	
	private:
		/******* private member objects *******/
		Config mConfig;

		/******* private member functions *******/
		// traverses the servers in mConfig and creates
			// listening sockets for each one in socketID
		// the sockets are created depending on the
			// values of hostname and port in each server
		void initSockets();

};
