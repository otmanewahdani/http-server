/* this file contains the prototypes of general utility
 	functions that are not associated to any classed
 * these functions usually have very small tasks and
 * can be used by different components regardless of their domain
 */

#pragma once

#include <string>
#include <sys/stat.h>

// returns true if path is a directory
bool isDir(const string& path);
