#include <iostream>
#include <fstream>
#include <vector>


void initialiseData(const char* filename, std::vector<int> &values);

int main() {
	std::vector<int> values = {};

	initialiseData("inplane.txt", values);
	
	for (int i = 0; i < 10; i++) {
		std::cout << values[values.size() - 1 -i] << std::endl;
	}

	return 0;
}

void initialiseData(const char* filename, std::vector<int> &values)
{
	std::ifstream myfile(filename);

	constexpr size_t bufferSize = 4 * 1024 * 1024;
	std::unique_ptr<char[]> buffer(new char[bufferSize]);

	auto getValue = [](char c) {return (int)c - 48;};

	while (myfile)
	{
		myfile.read(buffer.get(), bufferSize);

		for (size_t i = 0; i < bufferSize; i++) {
			int  a = getValue(buffer[i]);

			if (a >= 0 && a < 10) {
				while (getValue(buffer[i + 1]) >= 0 && getValue(buffer[i + 1]) < 10) {
					a *= 10;
					a += getValue(buffer[i + 1]);
					i++;
				}
				values.push_back(a);
			}
		}
	}

	myfile.close();
}