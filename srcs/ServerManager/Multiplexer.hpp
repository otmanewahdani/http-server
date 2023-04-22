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
#include <utils.hpp>
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
		// sets which are filled by FDs on each Multiplex operation
		static fd_set mReadFDset;
		static fd_set mWriteFDset;

		/******* private member functions *******/
		// removes all FDs from the member sets
		static void clearSets();

		// adds fd collections to their corresponding member sets
			// listenFDs and readFDs added to mReadFDset
			// writeFDs added to mWriteFDset
		// returns largest FD found in all collections
		static FD addFDsToSets
			(const FDCollection& listenFDs,
			const FDCollection& readFDs,
			const FDCollection& writeFDs);

		// adds all FDs in collection to FDset
		// returns largest FD found in collection
		static FD addFDCollectionToSet(const FDCollection& collec,
			fd_set& FDset);

		// removes from these collections all FDs that weren't
			// marked as ready by the multiplexer
		static void removeUnreadyFDs(FDCollection& listenFDs,
			FDCollection& readFDs, FDCollection& writeFDs);

		// removes FDs from collection that are not in FDset
		static void removeUnreadyFDs(FDCollection& collection,
			const fd_set& FDset);

};
