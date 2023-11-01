all: main run

main:
	mkdir -p build/main
	cp -r data/ build/main/
	gcc src/main.c -o build/main/main -lSDL2 -lSDL2_ttf
	
run:
	cd build/main; ./main; cd ../
