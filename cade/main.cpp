
#include "Application.hpp"
#include "Chip8.hpp"
int main(int argc, char* argv[])
{
	Application app;
	if (argc > 1)
		app.getCPU().loadROM(argv[1]);
	app.run();
	return 0;
}