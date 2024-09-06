#ifndef _UTILS_H
#define _UTILS_H

#include <vector>
#include <algorithm>

using namespace std;

int rand_range(int min, int max);

template<typename T>
void remove_index(vector<T>& container, int index){
	container.erase(container.begin() + index);
}

template<typename T>
T pop_index(vector<T>& container, int index){
	T value = move(container[index]);
	remove_index(container, index);
	return value;
}

template<typename T>
size_t count(const vector<T>& container, const T& value){
	return count(container.begin(), container.end(), value);
}

template<typename T>
void erase(vector<T>& container, const T& value){
	int found = 0;
	for(auto element: container){
		if(element != value) container[found++] = element;
	}
	while(container.size() > found) container.pop_back();
}

#endif
