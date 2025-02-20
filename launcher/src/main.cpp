#include <iostream>
#include "nightside/Application.hpp"

int main(int argc, char** argv) {

	printf("starting path : %s\n", std::filesystem::current_path().string().c_str());

	nightside::Application app{};
	app.run();

	return EXIT_SUCCESS;
}
