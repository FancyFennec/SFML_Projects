#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::experimental::filesystem;

int main() {
	for (auto p : fs::directory_iterator("TestFolder")) {
		std::cout << p.path() << std::endl;
	}
}