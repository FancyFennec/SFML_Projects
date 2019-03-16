#include <random>
#include <numeric>
#include <iostream>

std::random_device random;
std::default_random_engine engine(random());
std::uniform_int_distribution<int> uniformDist(-100, 100);

int main()
{

	double y = 260.34;

	int Y = (int)floor(y) & 255;

	std::cout << Y << "\n";

	std::vector<int> x = { 1,2,3 };

	std::iota(x.begin(), x.end(), -3);
	std::shuffle(x.begin(), x.end(), std::default_random_engine{ std::random_device{}() });

	for (auto p : x) {
		std::cout << p << "\n";
	}

	for (int i = 0; i < 20; i++) {
		std::cout << "Printing a random number: " << (float)uniformDist(engine) / 100 << "\n";
	}

	return 0;
}
