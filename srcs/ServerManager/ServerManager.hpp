/* This file contains the definition of the ServerManager class
 * This class manages all the operations of the webserver
 *  It acts as an intermediary between the Multiplexer module and 
 *  	the ClientHandler module
 *  It listens for incoming connections
 *  It manages existing clients connections and creates new clients
 *  It forwards clients' requests to the modules that handle requests
 *  It also forwards responses coming from
 *  	other modules to the corresponding clients
 */

#pragma once

#include <Config.hpp>
#include <Multiplexer.hpp>
#include <Network.hpp>
#include <MimeTypes.hpp>
#include <ClientHandler.hpp>

class ServerManager {

	public:
		/******* public alias types *******/
		typedef Config::Servers Servers;
		typedef ClientHandler::Socket Socket;
		typedef ClientHandler::FD FD;
		typedef std::map<Socket, ClientHandler> ClientHandlers;
		typedef Multiplexer::FDCollection FDCollection;

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
		ClientHandlers mClientsHandlers;

		// contains ClientHandlers that need Multiplexing on their FDs.
		std::map<FD, ClientHandler&> mFDMultiPlexQueries;

		// Collections of file descriptors that are retrieved from ClientHandlers
			// and Servers so that they can be passed to the Multiplexer 
		FDCollection mReadFDs;
		FDCollection mWriteFDs;
		FDCollection mListenFDs;

		/******* private member functions *******/
		// queries client Handlers for their state (if they need
			// multiplexing)
		// it also adds the servers sockets for listening multiplexing
		// and forwards the structures that contain their FDs to the Multiplexer
		// and lets them know if any action should be taken on their FDs
		// for servers if there incoming connections on their sockets, a new client
			// handler is added by calling addClientHandler()
		void manageClientHandlers();

		// checks the state of the client handlers by asking if they have
			// any multiplexing needs and if so it adds their FD to appropriate
			// collection (mReadFDs or mWriteFDs) and adds the handler to
			// mFDMultPlexQueries. These 3 member objects are cleared before use
		// ot if their connection was closed, it calls removeClientHandler()
		void queryClientHandlers();

		// adds server's sockets to mListenFDs after it is cleared
		void addServersForMultiplexing();

		// adds new client handlers for new incoming connections
			// on server's sockets that were marked as ready by
			// the multiplexer (listed in mListenFDs)
		void manageNewConnections();

		// gives the client handlers, whose FDs were passed
			// to the Multiplexer and they were marked as
			// ready, the permission to use that FD for an
			// I/O operation
		void informClientHandlers();

		// it removes a client handler from
			// mClientHandlers by looking up their ID
		void removeClientHandler(Socket ID);

		// creates a new client handler with ID
		void addClientHandler(Socket ID);

};
