/* This file contains the implementation of the Log class 
 * resonsible on logging into mLogfile
 * The log message has the following format:
 * [date time] [level of severity] [message] where the
 * level of severity represents the type of msg (info , error, warning ...)
 */

#include <Log.hpp>

const std::string errorNotice = "[ERROR] ";
const std::string infoNotice = "[INFO] ";

// log file creation
std::ofstream Log::mLogfile("./log_files/basic_logfile",
	std::ofstream::out | std::ofstream::trunc);

// Print the date and time in the format [YYYY-MM-DD HH:MM:SS]
	// by using time() fuction to get the current time 
	// then localtime() to convert it to local time expression
void Log::addTimeDate() {

	// Get the current time
	time_t rawTime = time(0);

  	// Convert to local time expression
  	tm* timeInfo = localtime(&rawTime);

  	// retrieve the date and time from tm struct returned by localtime()
		// and print it in the format [YYYY-MM-DD HH:MM:SS]
		// by appending the characters "[ ]-:" to timeInfo struct members
  	mLogfile << "["
			<< timeInfo->tm_year + 1900 << '-'
            << timeInfo->tm_mon + 1 << '-'
            << timeInfo->tm_mday << ' '
            << timeInfo->tm_hour << ':'
            << timeInfo->tm_min << ':'
            << timeInfo->tm_sec << "] ";

}

void Log::socketBinding(const Socket socket) {

	// writes to stream the current time
		// in [YYYY-MM-DD HH:MM:SS] format
	addTimeDate();

	try {
		// retrieve the host:port from the server socket
		const std::string serverName 
			= Network::getSocketServerName(socket);
		
		// log that the server is listening
			// on the retrieved host:port 
		mLogfile << "[INFO] Server listening on "
			<< serverName << ", with socket id = " 
			<< socket << "\n";
	}
	catch(const std::exception& e) {
		// log that the server is listening on some
			// unknown host:port if the retrieval fails
		mLogfile << "[ERROR] server Listening on "
			<< "some unknown host:port" << " with socket id = "
			<< socket << ", " << e.what() << "\n";
	}
							
}

void Log::connectionEstablished(const Socket socket) {

	// writes to stream the current time
		// in [YYYY-MM-DD HH:MM:SS] format
	addTimeDate();

	try {
		// retrieve the host:port of client from socket
		const std::string clientName 
			= Network::getSocketClientName(socket);

		try {
			// retrieve the host:port from the server socket
			const std::string serverName 
			= Network::getSocketServerName(socket);

			// log that the new connection identified 
				// by the retrieved client host:port is 
				// established on the retrieved server host:port
			mLogfile << "[INFO] " << "new connection established from "
				<< clientName << " on server " << serverName << "\n";
		}
		catch(const std::exception& e) {
			// if it fails to retrieve the server name
				// log the new connetion with the client name only
			mLogfile << "[ERROR] " << "new connection established from "
				<< clientName << ", " << e.what() << "\n";
		}
	}
	catch(const std::exception& f) {
		try {
			// retrieve the host:port from the server socket
			const std::string serverName 
			= Network::getSocketServerName(serverSocket);

			// if it fails to retrieve the client name
				//identify the new connection with server name only
			mLogfile << "[ERROR] " << "new connection established on server " 
			<< serverName << ", " << f.what() <<"\n";
		}
		catch(const std::exception& e) {
			// if it fails to retrieve the server name
				// and client name
			mLogfile << "[ERROR] " << "new connection established, "
			<< f.what() << "and " << e.what() << "\n";
		}	
	}

}

void Log::request(const Socket socket,
	const std::string& uri) {

	// writes to stream the current time
		// in [YYYY-MM-DD HH:MM:SS] format
	addTimeDate();

	try {
		// retrieve the host:port from client socket
		const std::string clientName 
			= Network::getSocketClientName(socket);

		// logs that the server received a request from
			//the client identified by the retrieved name
		mLogfile << "[INFO] " << "request ("<< uri 
		<<") received from " << clientName << "\n";
	}
	catch(const std::exception& e) {
		// in case of retrieval failure , log that
			// the server received a request
			// without specifying the client name
		mLogfile << "[ERROR] " << "request ("<< uri 
		<<") received by server, " << e.what() << "\n";
	}

}

void Log::response(const Socket socket) {

	// writes to stream the current time
		// in [YYYY-MM-DD HH:MM:SS] format
	addTimeDate();

	try {
		// retrieve the host:port from client socket
		const std::string clientName
		= Network::getSocketClientName(socket);

		// log that the server sent a reponse to
			//the client identified by the retrieved name
		mLogfile << "[INFO] " << "response sent to  "
		<< clientName << "\n";
	}
	catch(const std::exception& e) {
		// in case of retrieval failure , log that
			// the server sent a response
			// without specifying The recipient
		mLogfile << "[ERROR] " << "response sent by server, "
		<< e.what() << "\n";
	}

}

void Log::connectionClosed(const Socket socket) {

	// writes to stream the current time
		// in [YYYY-MM-DD HH:MM:SS] format
	addTimeDate();

	try {
		// retrieve the host:port from client socket
		const std::string clientName
		= Network::getSocketClientName(socket);

		// log that the connection with the client 
			//identified by the retrieved name is closed
		mLogfile << "[INFO] connection closed for client "
		<< clientName << "\n";
	}
	catch(const std::exception& e) {
		// in case of retrieval failure , log that
			// the server closed a connection
			// without specifying The client
		mLogfile << "[ERROR] " << "connection closed, "
		<< e.what() << "\n";
	}

}

void Log::error(const std::string& errorMsg) {

	// writes to stream the current time
		// in [YYYY-MM-DD HH:MM:SS] format
	addTimeDate();

	mLogfile << "[ERROR] " << errorMsg << "\n";

}

void Log::logClientServerOperation(const Soket socket,
	const std::string& op, const std::string& clientPrep,
	const std::string& serverPrep) {

	const std::string socketLog =
		"on socket " + std::to_string(socket)
		+ "; ";

	// tries to get both client and server names
	try {
		
		const std::string clientName 
			= Network::getSocketClientName(socket);

		// after successfully getting client name
			// it tries to get server name
		try {
			const std::string serverName 
				= Network::getSocketServerName(socket);

			addTimeDate();

			mLogfile << "[INFO] " << "on socket " << socket << ", "
				<< clientName << " on server " << serverName << "\n";
		}
		// if it fails to retrieve the server name logs the
			// operation with the client name only
		catch(const std::exception& e) {
			mLogfile << "[ERROR] " << "new connection established from "
				<< clientName << ", " << e.what() << "\n";
		}

	}
	// couldn't get any names
	catch(const std::exception& f) {

		// so it tries to get only server name
		try {

			const std::string serverName 
				= Network::getSocketServerName(serverSocket);

			// identifies the new connection with server name only
			mLogfile << "[ERROR] " << "new connection established on server " 
			<< serverName << ", " << f.what() <<"\n";
		}
		// still couldn't get any names
		catch(const std::exception& e) {
			// logs the operation with no names
			mLogfile << "[ERROR] " << "new connection established, "
				<< f.what() << "and " << e.what() << "\n";
		}

	}

}
