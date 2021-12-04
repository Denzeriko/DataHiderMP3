#include "fileManipulator.h"
#include <iostream>

// ENCODE
// ./run.exe e original.mp3      filetocrypt.txt  key

// DECODE
// ./run.exe d crypted_file.mp3  out.txt          key

//CHECK FILE FOR DATA
// ./run.exe c crypted_file.mp3     

//unsigned char key[16] = "Vmf7jDGsCWXXX78"; key must look like that

int main(int argc, char** argv) {
	if (argc < 1)
		return 0;

	FILESYS filesys;
	filesys.init();
	std::cout << "Running FILESYS\n";

	switch (*argv[1]) {
		case 'e':
			filesys.encodeInput(argv[2], argv[3], argv[4]);
			break;
		case 'd':
			filesys.decodeInput(argv[2], argv[3], argv[4]);
			break;
		case 'c':
			filesys.checkData(argv[2]);
			break;
	}
	
	return 0;
}