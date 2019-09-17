#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
#include "TestClass.h"
#include "msgpack.hpp"
namespace fs = std::experimental::filesystem;

int main() {

	std::ofstream ostrm("MyFile.jns", std::ios::out | std::ios::binary);
	std::vector<int> a = { 1, 2, 3 };
	
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, a);
	std::string data = sbuf.data();
	ostrm << data;
	ostrm.close();

	std::ifstream istrm("MyFile.jns", std::ios::in | std::ios::binary);

	istrm.seekg(0, std::ifstream::end);
	int size = istrm.tellg() / sizeof(char);
	char* buffer = new char(size);
	istrm.seekg(0, std::ifstream::beg);

	istrm.read((char*)buffer, sizeof(size));
	std::string data_back(buffer);

	// get your data safely back
	msgpack::unpacked msg = msgpack::unpack(data_back.data(), data_back.size());
	msgpack::object obj = msg.get();

	// now convert the bytes back to your objects
	std::vector<int> b;
	obj.convert(b);
	for (int x : b) {
		std::cout << x << std::endl;
	}
}
