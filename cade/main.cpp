
#include "Application.hpp"

int main(int argc, char* argv[])
{

	Application app;
	if (argc > 1)
		app.loadFile(argv[1]);
	app.run();
	return 0;
}