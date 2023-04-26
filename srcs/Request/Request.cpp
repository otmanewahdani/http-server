/* this file contains the implementation of the Request class*/

#include <Request.hpp>

size_t Request::readSize = 1024;

size_t Request::requestLineSizeLimit = 2048;

size_t Request::headersSizeLimit = 8192;
