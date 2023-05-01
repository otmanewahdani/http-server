/* This file contains the definition of the ServerManager class
 * This class manages all the operations of the webserver
 *  It acts as an intermediary between the Multiplexer module and 
 *  	the ClientHandler module
 *  It listens for incoming connections
 *  It manages existing clients connections and creates new clients
 */

#pragma once

#include <Config.hpp>
#include <Log.hpp>
#include <Multiplexer.hpp>
#include <Network.hpp>
#include <MimeTypes.hpp>
#include <ClientHandler.hpp>
#include <RequestHeaders.hpp>

class ServerManager {

	public:
		/******* public alias types *******/
		typedef Config::Servers Servers;
		typedef ClientHandler::Socket Socket;
		typedef std::map<Socket, ClientHandler> ClientHandlers;
		typedef Multiplexer::FDCollection FDCollection;
		typedef Config::ServerRef ServerRef;
		typedef Config::ConstServerRef ConstServerRef;

		/******* public member functions *******/
		// takes a configuration file path
			// and passes it to Config
		// calls Network::initServersSockets()
			// and initializeStaticData()
		ServerManager(const char* configFileName);

		// starts the operations of the server
			// (handling clients, requests, etc..)
		void start();

		void printConfig();

		const std::string& getTmpFilesDir();
	
	private:
		/******* private member objects *******/
		Config mConfig;

		// the managed servers in mConfig
		const Servers& mServers;

		// associates extensions with their mime types
		MimeTypes mMimeTypes;

		// a collection of handlers for each client
		ClientHandlers mClientHandlers;

		// Collections of sockets that are retrieved from ClientHandlers
			// and Servers so that they can be passed to the Multiplexer 
		FDCollection mReadFDs;
		FDCollection mWriteFDs;
		FDCollection mListenFDs;

		// directory where the temporary files of
			// the program will be created
		static const std::string tmpFilesDir;

		/******* private member functions *******/
		// queries client Handlers for their state (if they need
			// multiplexing)
		// it also adds the servers sockets for listening multiplexing
		// and forwards the structures that contain their sockets to the Multiplexer
		// and lets them know if any action should be taken on their sockets
		// for servers if there incoming connections on their sockets, a new client
			// handler is added by calling addClientHandler()
		void manageClientHandlers();

		// Checks the state of the client handlers by asking if they have
			// any multiplexing needs and if so it adds their socket to appropriate
			// collection (mReadFDs or mWriteFDs) These 2 member objects are
			// cleared before use
		// Or if their connection was closed, it calls removeClientHandler()
		void queryClientHandlers();

		// adds server's sockets to mListenFDs after it is cleared
		void addServersForMultiplexing();

		// adds new client handlers for new incoming connections
			// on server's sockets that were marked as ready by
			// the multiplexer (listed in mListenFDs)
		// the new socket id is made non-blocking
		void manageNewConnections();

		// gives the client handlers, whose sockets were passed
			// to the Multiplexer and they were marked as
			// ready, the permission to use that socket for an
			// I/O operation
		void informClientHandlers();

		// used by parameterless informClientHandlers() to
			// inform client handlers about a specific type of
			// FD collection. For example, it can be used to
			// inform handlers who made a read multiplex
			// query by passing it ReadFDs
		void informClientHandlers(FDCollection& FDs);

		// returns a non-blocking socket for a new
			// incoming connection on a listening
			// socket that's been marked as ready
			// by a multiplexer
		// throws std::runtime_error on error
		Socket getNewConnectionSock(Socket listenSock);

		// it removes a client handler from
			// mClientHandlers by looking up their ID
		void removeClientHandler(Socket ID);

		// creates a new client handler associated to a server that has
			// the SocketID of serverID
		// throws std::invalid_argument in case a client handler with
			// clientID exists already
		void addClientHandler(Socket clientID, Socket serverID);

		// finds client handler by looking up its ID
			// throws std::invalid_argument in case a
			// ClientHandler with that ID wasn't found
		ClientHandler& getClientHandler(Socket ID);

		// functions in different modules that initialize
			// static structures will be called here
		static void initializeStaticData();

		// creates the temporary files directory
			// if it doesn't exist
		// throws std::runtime_error on error
		void makeTmpFilesDir();

};
