all:
	@clear

	@echo "----------------------------------------------------------------------"
	@echo "Building program..."
	@echo "----------------------------------------------------------------------"

	g++ -c src/*.cpp -std=c++11
	g++ -o SFApp *.o -l SDL2 -l SDL2_image

	@echo "----------------------------------------------------------------------"
	@echo "Build finished. If any errors occured, they will show above."
	@echo "You can run a succesful build with ./SFApp"
	@echo "----------------------------------------------------------------------"