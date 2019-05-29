#include "TestClass.h"
#include "FunctionTest.h"
#include <map>


int main() {

	int a = 2;

	struct Node {
		TestClass child;
		TestClass Parent;

		Node(TestClass newChild) {
			child = newChild;
		}
	};

	Node testNode(TestClass(1));

	std::cout << testNode.child.number << "\n";

	std::cin >> a;
	return 0;
}