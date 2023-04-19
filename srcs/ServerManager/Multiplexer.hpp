/* this file contains the definition of Multiplexer class 
 * It manages I/O multiplexing. It enables checking of multiple
 * file descriptors for new connections, read and write readiness
 * by making the program sleep until one or more of these file descriptors
 * are ready for the above-mentioned operations.
 * This functionality can be obtained by calling checkFDsForEvents and passing
 * it 3 types of FD collections: ListenFDs, ReadFDs, WriteFDs and the Multiplexer
 * will remove the FDs from those collections that have no upcoming events
*/

#pragma once

#include <sys/select.h>
#include <vector>

class Multiplexer {

	public:
		/******* alias types *******/
		typedef int FD;
		typedef std::vector<FD> FDCollection;

		/******* public member functions *******/
		// checks if the following fd collections are respectively ready
			// for these operations: accepting new incoming connections,
			// reading and writing.
		// It leaves the FDs in a Collection if they are ready and removes them
			// if they are not
		static void checkFDsForEvents(FDCollection& listenFDs,
			FDCollection& readFDs, FDCollection& writeFDs);

	private:
		/******* private member objects *******/
		static fd_set mReadFDs;
		static fd_set mWriteFDs;

 };