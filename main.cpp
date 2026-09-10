#include "MemTypes.hpp"
#include <iostream>
using namespace std;

int main() {
	int* ptr = (int*) alloc_mem(sizeof(int) * 268435456);
	for (int i = 0; i < 268435456; i++) {
		ptr[i] = i;
	}
	system("pause");
	for (int i = 0; i < 268435456; i++) {
		cout << ptr[i] << "\n";
	}
	system("pause");
	free_mem(ptr);
	system("pause");
	return 0;
}