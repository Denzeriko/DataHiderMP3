#include <filesystem>
#include <fstream>
#include <iostream>
#include <windows.h>
#include "aes.h"

class FILESYS {
public:
	int init() {
		printf(":::::::::::::::::::\n");
		printf("::DENZ MP3 AES128::\n");
		printf("::      2021     ::\n");
		printf(":::::::::::::::::::\n");
		return 1;
	}

	void fixKey(char* key) {
		for (int i = 0; i < 16; i++)
		{
			if (key[i] == '\0')
			{
				for (int x = i; x < 16; x++)
				{
					key[x] = 'x'; //usefull to make easier keys. But very unsecure.
				}
				break;
			}
		}
	}

	void checkData(char* mainFile) {
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
			printf("Some data has found!\n");
			return;
		}
		printf("No data was found!\n");
	}

	void encodeInput(char* mainFile, char* dataFile, char* _key) {
		char key[16]; // I have function up there, but i don't have exp. to make it. :(
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

		printf("::ENCODE::\n");
		
		printf("Reading: %s\n", mainFile);
		std::ifstream is(mainFile, std::ios::out | std::ios::binary);
		
		printf("Reading: %s\n", dataFile);
		std::ifstream as(dataFile, std::ios::out | std::ios::binary);

		std::stringstream buffer2;
		buffer2 << as.rdbuf();

		unsigned int plainLen = buffer2.str().size();
		unsigned int outLen = 0;

		AES aes(128);
		BYTE* out = aes.EncryptECB((unsigned char*)buffer2.str().c_str(), plainLen, (unsigned char*)key, outLen);

		std::stringstream buffer;
		buffer << is.rdbuf() << "VYBQ@!^BEAW&D^";

		for (size_t i = 0; i < plainLen; i++) // CRINGE DUDE any another way?
		{
			buffer << out[i];
		}

		printf("Main size  : %zd\n", buffer.str().size());
		printf("Concating files\n");
		printf("Buffer size: %zd | [%zdkb] ~aprox~\n", buffer.str().size() + plainLen, (buffer.str().size() + plainLen)/1024); //cringe way to find path / fix?

		std::ofstream f2("./out.mp3", std::ios::out | std::ios::binary);
		f2.write(buffer.str().c_str(), buffer.str().size());
		f2.close();
	}

	void decodeInput(char* mainFile, char* dataFile, char* _key) {
		char key[16]; // I have function up there, but i don't have exp. to make it. :(
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

		printf("::DECODE::\n");
		
		printf("Reading: %s\n", mainFile);
		std::ifstream is(mainFile, std::ios::out | std::ios::binary);

		std::stringstream buffer;
		buffer << is.rdbuf();

		printf("PASS #1\n");

		std::string s = buffer.str();
		std::string delimiter = "VYBQ@!^BEAW&D^";

		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			s.erase(0, pos + delimiter.length());
		}
		printf("PASS #2\n");

		unsigned int plainLen = s.size();
		unsigned int outLen = 0;

		AES aes(128);
		BYTE* out = aes.DecryptECB((unsigned char*)s.c_str(), plainLen, (unsigned char*)key);

		printf("SIZE      :%zd\n", s.size());

		std::ofstream f2(dataFile, std::ios::out | std::ios::binary);
		f2.write((char*)out, plainLen);
		f2.close();
	}
};