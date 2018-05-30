#include <iostream>
#include <vector>
#include "fakeVector.h"
#include "timer.h"

template <typename T>
void printVector(const T& vector){
	for (auto i = 0; i < vector.size(); i++)
		std::cout << vector[i] << ' ';
	std::cout << std::endl;
}

template <typename T>
void showStats(const T& vector){
	std::cout << "size: " << vector.size() << " capacity: " << vector.capacity() << std::endl;
}



// move
template <typename T>
double testFunction(unsigned int testCount){
	// T b{1, 3, 5, 6, 9, 7, 6, 1, 2, 3, 4, 6};
	int bl;
	T a;
	Timer start;
	const std::string somestring = "asdgds";
	for (int i = 0; i < testCount; i++){
		a.push_back(i);
	}	
	return start.elapsed();
}

// class someClass{
// 	std::string name;
// 	std::string lastname;
// public:
// 	someClass(std::string n = "", std::string l = "") : 
// 		name(n),
// 		lastname(l)
// 		{};
// };

int main(){	
	const int operation_count = 1e7;
	const int test_count = 1;
	double fake = 0;
	double real = 0;
	for (int i = 0; i < test_count; i++){
		fake += testFunction<fake::vector<int>>(operation_count);
		real += testFunction<std::vector<int>>(operation_count);
	}

	std::cout << "Average fake time: " << fake/test_count << 's' << std::endl;
	std::cout << "Average real time: " << real/test_count << 's' << std::endl;
	std::cout << (1-fake/real)*100 << "\% performance increase" << std::endl;
	
 	return 0;
}