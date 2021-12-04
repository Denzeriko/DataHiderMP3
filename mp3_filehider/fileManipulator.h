#include <filesystem>
#include <fstream>
#include <iostream>
#include <windows.h>
#include "aes.h"

class FILESYS {
public:
	int init() {
		printf("\n:::::::::::::::::::\n");
		printf("::DENZ MP3 AES128::\n");
		printf("::      2021     ::\n");
		printf(":::::::::::::::::::\n\n");
		printf("FILESYS: Good!\n\n");
		return 1;
	}

	void fixKey(char* key, char* _key) {
		for (int i = 0; i < 16; i++)
		{
			key[i] = _key[i];
			if (_key[i] == '\0')
			{
				for (int x = i; x < 16; x++)
				{
					key[x] = 'x'; //usefull to make easier keys. But very unsecure.
				}
				break;
			}
		}
	}

	int checkData(char* mainFile) {
		printf("Reading: %s\n", mainFile);
		std::ifstream as(mainFile, std::ios::out | std::ios::binary);
		std::stringstream ss;
		ss << as.rdbuf();
		std::string s = ss.str();
		std::string delimiter = "VYBQ@!^BEAW&D^";

		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			s.erase(0, pos + delimiter.length());
			printf("Some data was found!\n");
			return 0;
		}
		printf("No data was found!\n");
		return 1;
	}

	void encodeInput(char* mainFile, char* dataFile, char* _key) {
		char key[16];
		fixKey(key, _key);

		printf("::ENCODE::\n");

		//file container
		printf("Reading: %s\n", mainFile);
		std::ifstream mainStream(mainFile, std::ios::out | std::ios::binary);
		std::stringstream mainBuffer;
		mainBuffer << mainStream.rdbuf() << "VYBQ@!^BEAW&D^";  //random bytes to get delimer in decoding

		//what to write
		printf("Reading: %s\n", dataFile);
		std::ifstream dataStream(dataFile, std::ios::out | std::ios::binary);
		std::stringstream dataBuffer;
		dataBuffer << dataStream.rdbuf();
		
		AES aes(128);
		unsigned int aesLen;
		BYTE* out = aes.EncryptECB((unsigned char*)dataBuffer.str().c_str(), dataBuffer.str().size(), (unsigned char*)key, aesLen);
		printf("PASS #1 Data was crypted\n");

		for (size_t i = 0; i < dataBuffer.str().size(); i++) { // Bruh, any another way?
			mainBuffer << out[i];
		}
		printf("PASS #2 Data injected in mp3\n");

		std::ofstream f2("./out.mp3", std::ios::out | std::ios::binary);
		f2.write(mainBuffer.str().c_str(), mainBuffer.str().size());
		f2.close();

		printf("PASS #3 File was written\n");
	}

	void decodeInput(char* mainFile, char* dataFile, char* _key) {
		char key[16];
		fixKey(key, _key);

		printf("::DECODE::\n");

		printf("Reading: %s\n", mainFile);
		std::ifstream mainStream(mainFile, std::ios::out | std::ios::binary);

		std::stringstream mainBuffer;
		mainBuffer << mainStream.rdbuf();

		printf("PASS #1 File was read\n");
		std::string mainBufferS = mainBuffer.str();
		std::string delimiter = "VYBQ@!^BEAW&D^";

		size_t pos = 0;
		std::string token;
		while ((pos = mainBufferS.find(delimiter)) != std::string::npos) {
			token = mainBufferS.substr(0, pos);
			mainBufferS.erase(0, pos + delimiter.length());
		}

		printf("PASS #2 Found start of data\n");

		AES aes(128);
		unsigned int aesLen = mainBufferS.size();
		BYTE* out = aes.DecryptECB((unsigned char*)mainBufferS.c_str(), aesLen, (unsigned char*)key);

		printf("PASS #3 Data was decrypted\n");

		std::ofstream dataStream(dataFile, std::ios::out | std::ios::binary);
		dataStream.write((char*)out, aesLen);
		dataStream.close();

		printf("PASS #4 File was writen\n");
	}
};