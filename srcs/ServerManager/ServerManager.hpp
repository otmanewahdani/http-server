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
#include <Network.hpp>
#include <MimeTypes.hpp>
#include <Client.hpp>

class ServerManager {

	public:
		/******* public alias types *******/
		typedef Config::Servers Servers;
		typedef Client::Socket Socket;
		typedef std::map<Socket, ClientHandler> ClientHandlers;

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

		// the managed servers in mConfig
		const Servers& mServers;

		// associates extensions with their mime types
		MimeTypes mMimeTypes;

		// a collection of handlers for each client
		Clients mClientsHandlers;

		/******* private member functions *******/
		// waits for new connections on the mServers's sockets
			// and turn those connections into clients
		void manageClients();

};
