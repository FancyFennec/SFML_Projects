#include <iostream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
namespace fs = std::experimental::filesystem;

int main() {
	for (auto p : fs::directory_iterator("./TestFolders")) {
		bool folder_found = false;
		for (auto s : fs::directory_iterator(p.path())) {
			if (!folder_found) {
				folder_found = true;
				std::cout << std::string("Hooray! There were files in folder ").append(p.path().string()) << std::endl;
			}
			std::cout << s.path().string().data() << std::endl;
		}
		if (!folder_found) {
			std::cout << std::string("No files found in ").append(p.path().string()) << std::endl;
		}
		
	}
	std::cout << "bla" << std::endl;

	return 0;
}