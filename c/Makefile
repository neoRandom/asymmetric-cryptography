default:
	gcc src/*.c -o build/main.out -I include/

execute:
	gcc src/*.c -o build/main.out -I include/
	./build/main.out

release:
	gcc src/*.c -o build/main.out -I include/ -O3 -static -static-libgcc
