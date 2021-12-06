#include <filesystem>
#include <fstream>
#include <iostream>
#include <windows.h>

#include "aes.h"
#include "miniz.h"

class FILESYS {
public:
	std::string delimiter = "RprjH74i94Wf4RFrW5EP!$@";  //random bytes to get delimer in decoding

	FILESYS() {
		printf("\n:::::::::::::::::::\n");
		printf("::DENZ MP3 AES128::\n");
		printf("::      2021     ::\n");
		printf(":::::::::::::::::::\n\n");
		printf("FILESYS: CREATED!\n\n");
	}
	~FILESYS() {
		printf("\nFILESYS: DELETED!\n\n");
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
		printf("PASS #1 Reading: %s\n", mainFile);
		std::ifstream as(mainFile, std::ios::out | std::ios::binary);
		std::stringstream ss;
		ss << as.rdbuf();
		std::string s = ss.str();

		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			s.erase(0, pos + delimiter.length());
			printf("Signature found!\n");
			return 0;
		}
		printf("ERR# (No signature was found!)\n");
		return 1;
	}

	void encodeInput(char* mainFile, char* dataFile, char* _key) {
		char key[16];
		fixKey(key, _key);

		printf("::ENCODE::\n");

		std::string hashStr{ _key }; //It's not work storing key or using key as hash in encode. 
		std::hash<std::string> hasher; //Just to check main key. I guess it's bad idea, but pretty cool feature.
		unsigned int hashResult = hasher(hashStr) + hasher(hashStr);

		//file container
		printf("PASS #1 Reading %s ", mainFile);
		std::ifstream mainStream(mainFile, std::ios::in | std::ios::binary);
		if (!mainStream.good()) {
			printf("ERR!\n");
			return;
		}
		std::stringstream mainBuffer;
		mainBuffer << mainStream.rdbuf() << delimiter << hashResult << hashResult;
		mainStream.close();
		printf("OK!\n");

		//What to write
		printf("PASS #2 Reading %s ", dataFile);
		std::ifstream dataStream(dataFile, std::ios::in | std::ios::binary);
		if (!dataStream.good()) {
			printf("ERR!\n");
			return;
		}	
		std::stringstream dataBuffer;
		dataBuffer << dataStream.rdbuf();
		dataStream.close();
		printf("OK!\n");

		uLong src_len = dataBuffer.str().size();
		uLong cmp_len = compressBound(src_len);
		printf("PASS #3 Compressing data %u to ", src_len);
		unsigned char* pCmp = (mz_uint8*)malloc((size_t)cmp_len);
		int comp_status = compress(pCmp, &cmp_len, (const unsigned char*)dataBuffer.str().c_str(), src_len);
		if (comp_status != 0) {
			printf("ERR!\n");
			return;
		}
		printf("%u ", cmp_len);
		printf("OK!\n");

		printf("PASS #4 Crypting data ");
		AES aes(128);
		unsigned int aesLen;
		BYTE* out = aes.EncryptECB(pCmp, cmp_len, (unsigned char*)key, aesLen);
		printf("OK!\n");

		printf("PASS #5 Injecting data to file ");
		mainBuffer.write((char*)out, aesLen);
		std::ofstream outputStream("./out.mp3", std::ios::out | std::ios::binary);
		if (!outputStream.good()) {
			printf("ERR!\n");
			return;
		}
		outputStream.write(mainBuffer.str().c_str(), mainBuffer.str().size());
		outputStream.close();

		printf("OK!\n");
	}

	void decodeInput(char* mainFile, char* dataFile, char* _key) {
		char key[16];
		fixKey(key, _key);

		printf("::DECODE::\n");

		std::string hashStr{ _key };
		std::hash<std::string> hasher;
		unsigned int hashResult = hasher(hashStr) + hasher(hashStr);

		printf("PASS #1 Read file ");
		std::ifstream mainStream(mainFile, std::ios::in | std::ios::binary);
		if (!mainStream.good()) {
			printf("ERR!\n");
			return;
		}
		std::stringstream mainBuffer;
		mainBuffer << mainStream.rdbuf();
		mainStream.close();
		printf("OK!\n");
		
		printf("PASS #2 Searching for signature ");
		size_t pos = 0;
		std::string token;
		std::string mainBufferS = mainBuffer.str();

		bool checkSignature = false;
		bool checkKeyPassword = false;
		while ((pos = mainBufferS.find(delimiter)) != std::string::npos) { //todo: rework this part...
			token = mainBufferS.substr(0, pos);
			checkSignature = true;
			mainBufferS.erase(0, pos + std::to_string(hashResult).length());
		}
		if (!checkSignature)
		{
			printf("ERR# (No signature was found!)\n");
			return;
		}
		while ((pos = mainBufferS.find(std::to_string(hashResult))) != std::string::npos) {
			token = mainBufferS.substr(0, pos);
			checkKeyPassword = true;
			mainBufferS.erase(0, pos + std::to_string(hashResult).length());
		}
		if (!checkKeyPassword)
		{
			printf("ERR# (Wrong password!)\n");
			return;
		}
		
		printf("OK!\n");
		//printf("OK! %d | %d\n", checkSignature, checkKeyPassword);

		printf("PASS #3 Decrypting data ");
		AES aes(128);
		unsigned int aesLen = mainBufferS.size();
		BYTE* out = aes.DecryptECB((unsigned char*)mainBufferS.c_str(), aesLen, (unsigned char*)key);
		printf("OK!\n");

		printf("PASS #4 Decompressing data ");
		uLong src_len = 100000000;//mainBufferS.size(); // HOW I MUST KNOW ORIGINAL SIZE?
		uLong uncomp_len = src_len;
		unsigned char* pUncomp = (mz_uint8*)malloc((size_t)src_len);
		int comp_status = uncompress(pUncomp, &uncomp_len, out, src_len);
		if (comp_status != 0) {
			printf("ERR! %d\n", comp_status);
			return;
		}
		printf("OK!\n");

		printf("PASS #4 Writing to file ");
		std::ofstream dataStream(dataFile, std::ios::out | std::ios::binary);
		if (!dataStream.good()) {
			printf("ERR!\n");
			return;
		}
		dataStream.write((char*)pUncomp, uncomp_len);
		dataStream.close();
		printf("OK!\n");
	}
};