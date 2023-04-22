/* This file contains the implementation of the Log class 
 * resonsible on logging into mLogfile
 * The log message has the following format:
 * [date time] [level of severity] [message] where the
 * level of severity represents the type of msg (info , error, warning ...)
 */

#include <Log.hpp>

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

	std::string serverName;

	// print the current time in format [YYYY-MM-DD HH:MM:SS]
	addTimeDate();

	try {
		// retrieve the host:port from the server socket
		serverName = Network::getSocketServerName(socket);

		// log that the server is listening
			// on the retrieved host:port 
		mLogfile << "[INFO] Server listening on "
		<< serverName << "\n";
	}
	catch(const std::exception& e) {
		// log that the server is listening on the 
			// expected host:port if the retrieval fails
		mLogfile << "[INFO] " <<e.what() 
		<< ", server Listening on the expected host:port\n";
	}
							
}

void Log::connectionEstablished(const Socket socket) {

	std::string clientName;

	// print the current time in format [YYYY-MM-DD HH:MM:SS]
	addTimeDate();

	try {
		// retrieve the host:port from client socket
		clientName = Network::getSocketClientName(socket);

		// log that the new connection identified 
			//by the retrieved host:port is established
		mLogfile << "[INFO] " << "New connection established from "
		<< clientName << "\n";
	}
	catch(const std::exception& e) {
		// in case of retrieval failure , log the new
			// connection without providing the client name
		mLogfile << "[INFO] " << "New connection established\n";
	}

}

void Log::request(const Socket socket) {

	std::string clientName;

	// print the current time in format [YYYY-MM-DD HH:MM:SS]
	addTimeDate();

	try {
		// retrieve the host:port from client socket
		clientName = Network::getSocketClientName(socket);

		// log that the server received a request from
			//the client identified by the retrieved name
		mLogfile << "[INFO] " << "Request received from  "
		<< clientName << "\n";
	}
	catch(const std::exception& e) {
		// in case of retrieval failure , log that
			// the server received a request
			// without specifying the client name
		mLogfile << "[INFO] " << "Request received by server\n";
	}

}

void Log::response(const Socket socket) {

	std::string clientName;

	// print the current time in format [YYYY-MM-DD HH:MM:SS]
	addTimeDate();

	try {
		// retrieve the host:port from client socket
		clientName = Network::getSocketClientName(socket);

		// log that the server sent a reponse to
			//the client identified by the retrieved name
		mLogfile << "[INFO] " << "Response sent to  "
		<< clientName << "\n";
	}
	catch(const std::exception& e) {
		// in case of retrieval failure , log that
			// the server sent a response
			// without specifying The recipient
		mLogfile << "[INFO] " << "Response sent by server\n";
	}

}

void Log::connectionClosed(const Socket socket) {

	std::string clientName;

	// print the current time in format [YYYY-MM-DD HH:MM:SS]
	addTimeDate();

	try {
		// retrieve the host:port from client socket
		clientName = Network::getSocketClientName(socket);

		// log that the connection with the client 
			//identified by the retrieved name is closed
		mLogfile << "[INFO] Connection closed for client "
		<< clientName << "\n";
	}
	catch(const std::exception& e) {
		// in case of retrieval failure , log that
			// the server closed a connection
			// without specifying The client
		mLogfile << "[INFO] " << "Connection closed\n";
	}

}

void Log::error(const std::string& errorMsg) {

	// print the current time in format [YYYY-MM-DD HH:MM:SS]
	addTimeDate();

	mLogfile << "[ERROR] " << errorMsg << "\n";

}