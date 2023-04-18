#include "MimeTypes.hpp"

int main()
{
	try
	{
		MimeTypes ob("./mime_types_files/mime.types");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}