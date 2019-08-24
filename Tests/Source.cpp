#include <iostream>
#include <vector>


int main() {

	std::vector<int> vec = { 1, 2, 3, 4, 5 ,6 };

	/*auto iter = vec.begin();
	while(iter != vec.end()){
		if (*iter % 2 == 1) {
			iter = vec.erase(iter);
		}
		else {
			iter++;
		}

	}*/

	auto iter = std::prev(vec.end());
	while (iter != std::prev(vec.begin())) {
		if (*iter % 2 == 1) {
			iter = vec.erase(iter);
		}
		else {
			iter--;
		}

	}

	for (auto nr : vec) {
		std::cout << nr << std::endl;
	}
	
}