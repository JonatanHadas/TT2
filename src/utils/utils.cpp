#include "utils.h"
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

int rand_range(int min, int max){
	static default_random_engine engine(system_clock::now().time_since_epoch().count());
	return uniform_int_distribution<int>(min, max - 1)(engine);
}
