#include "fileManipulator.h"

// ENCODE
// ./run.exe e original.mp3      filetocrypt.txt  key

// DECODE
// ./run.exe d crypted_file.mp3  out.txt          key

// CHECK FILE FOR DATA
// ./run.exe c crypted_file.mp3     

//unsigned char key[16] = "XXXXXXXXXXXXXXX"; key must look like that
//you can use any length to 16

int main(int argc, char** argv) {
	if (argc == 1)
		return 1;

	FILESYS* filesys = new FILESYS;
	switch (*argv[1]) {
		case 'e':
			filesys->encodeInput(argv[2], argv[3], argv[4]);
			break;
		case 'd':
			filesys->decodeInput(argv[2], argv[3], argv[4]);
			break;
		case 'c':
			filesys->checkData(argv[2]);
			break;
	}
	delete filesys;

	return 0;
}