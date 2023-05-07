#include <ServerManager.hpp>

int main(int ac, char** av) {

	(void) ac;

	try {

		// takes the config file path
			// in the first argument
		ServerManager manager(av[1]);
		manager.start();

	}
	catch (const std::exception& error) {

		std::cerr << "Exception: "
			<< error.what() << '\n';
		exit(EXIT_FAILURE);

	}

	exit(EXIT_SUCCESS);

}
