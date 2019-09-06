#include <iostream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
namespace fs = std::experimental::filesystem;

int main() {
	/*bool folder_found = false;
	for (auto p : fs::directory_iterator("./TestFolders")) {
		if (fs::is_directory(p)) {
			folder_found = true;
			bool files_found = false;
			std::cout << std::string("Hooray! Folder found: ").append(p.path().string().data()) << std::endl;
			for (auto s : fs::directory_iterator(p.path())) {
				if (!fs::is_directory(s)) {
					files_found = true;
					std::cout << std::string("Hooray! There were files in the folder: ").append(s.path().string()) << std::endl;
				}
				else {
					std::cout << std::string("Hooray! Found another folder. ") << std::endl;
				}
			}
			if (!files_found) {
				std::cout << std::string("No files found in: ").append(p.path().string().data()) << std::endl;
			}
		}
	}
	if (!folder_found) {
		std::cout << std::string("No files found in ./TestFolders") << std::endl;
	}*/

	char test[128] = "Hallo";
	strcpy_s(test, "Echo");

	std::cout << test << std::endl;

	return 0;
}