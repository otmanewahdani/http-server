/* this file contains the implementation of Multiplexer class 
 */

#include <Multiplexer.hpp>

// declares static member objects
fd_set Multiplexer::mReadFDset;
fd_set Multiplexer::mWriteFDset;

void Multiplexer::checkFDsForEvents(FDCollection& listenFDs,
	FDCollection& readFDs, FDCollection& writeFDs) {

	clearSets();
	// adds fd to member sets so they can be passed to select
	FD largestFD = addFDsToSets(listenFDs, readFDs, writeFDs);

	// waits until any of the FDs are ready for any I/O events
	if (select(largestFD + 1, &mReadFDset, &mWriteFDset,
		NULL, NULL) == -1) {

		throwErrnoException
			("failed to check fds for events");

	}

	// removes FDs from the FD collection arguments
		// that were not marked as ready by select
	removeUnreadyFDs(listenFDs, readFDs, writeFDs);

}

void Multiplexer::clearSets() {

	FD_ZERO(&mReadFDset);
	FD_ZERO(&mWriteFDset);

}

Multiplexer::FD Multiplexer::addFDsToSets
	(const FDCollection& listenFDs,
	const FDCollection& readFDs,
	const FDCollection& writeFDs) {

	FD largestFD =
		addFDCollectionToSet(listenFDs, mReadFDset);

	FD tmp =
		addFDCollectionToSet(readFDs, mReadFDset);

	if (tmp > largestFD)
		largestFD = tmp;

	tmp = addFDCollectionToSet(writeFDs, mWriteFDset);

	if (tmp > largestFD)
		largestFD = tmp;

	return largestFD;

}

Multiplexer::FD Multiplexer::addFDCollectionToSet
	(const FDCollection& collec, fd_set& FDset) {

	FD largestFD = -1;
	// iterates over fds and adds them to FDset
	// and saves largest fd found
	for (FDCollection::const_iterator
		fd = collec.begin();
		fd != collec.end(); ++fd) {
		
		FD_SET(*fd, &FDset);
		if (*fd > largestFD)
			largestFD = *fd;

	}

	return largestFD;

}

void Multiplexer::removeUnreadyFDs(FDCollection& listenFDs,
	FDCollection& readFDs, FDCollection& writeFDs) {

	// calls the other overload of this function
	removeUnreadyFDs(listenFDs, mReadFDset);

	removeUnreadyFDs(readFDs, mReadFDset);

	removeUnreadyFDs(writeFDs, mWriteFDset);

}

void Multiplexer::removeUnreadyFDs(FDCollection& collection,
	const fd_set& FDset) {

	for (FDCollection::iterator fd = collection.begin();
		fd != collection.end(); ) {

		// if fd is not in set (so it is no ready for I/O)
			// removes it from collection and gets next one
		// else just gets next one
		if (!FD_ISSET(*fd, &FDset))
			fd = collection.erase(fd);
		else
			++fd;

	}

}
