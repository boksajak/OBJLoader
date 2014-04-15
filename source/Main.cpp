
#include "OBJModelLoader.h"
#include "Timer.h"

#include <iostream>

int main(int argc, char* argv[]){
	OBJModelLoader objModelLoader;

	if (argc < 2) {
		std::cout << "Specify file to load" << std::endl;
		return -1;
	}

	objModelLoader.Initialize();

	std::cout << "Loading " << argv[1] << "..." << std::endl;

	Timer timer;

	objModelLoader.LoadOBJModel("", argv[1]);

	std::cout << "Loading finished in " << timer.Stop() << "s." << std::endl;

	objModelLoader.Deinitialize();

	getchar();
}