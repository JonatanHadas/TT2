#ifndef _SERIALIZATION_H
#define _SERIALIZATION_H

#include <iostream>
#include <ostream>
#include <istream>
#include <vector>

using namespace std;

template<typename T>
class Serializer{
public:
	static void serialize(ostream& output, const T& value){
		value.serialize(output);
	}
	
	static T deserialize(istream& input){
		return T::deserialize(input);
	}
};

template<typename T>
void serialize_value(ostream& output, T value){
	Serializer<T>::serialize(output, value);
}

template<typename T>
T deserialize_value(istream& input){
	return Serializer<T>::deserialize(input);
}

template<typename T, int size>
void serialize_int(ostream& output, T value){
	char data[size];
	for(unsigned int i = 0; i < size; i++){
		data[i] = (value >> (i << 3)) & 0xff;
	}
	output.write(data, size);
}

template<typename T, int size>
T deserialize_int(istream& input){
	char data[size];
	input.read(data, size);
	
	T result;
	for(unsigned int i = 0; i < size; i++){
		result |= data[i] << (i << 3);
	}
	return result;
}


template<>
class Serializer<int>{
public:
	static void serialize(ostream& output, int value) { serialize_int<int, 4>(output, value); }
	static int deserialize(istream& input) { return deserialize_int<int, 4>(input); }
};
template<>
class Serializer<unsigned int>{
public:
	static void serialize(ostream& output, unsigned int value) { serialize_int<unsigned int, 4>(output, value); }
	unsigned static int deserialize(istream& input) { return deserialize_int<unsigned int, 4>(input); }
};
template<>
class Serializer<char>{
public:
	static void serialize(ostream& output, char value) { serialize_int<char, 1>(output, value); }
	static char deserialize(istream& input) { return deserialize_int<char, 1>(input); }
};
template<>
class Serializer<unsigned char>{
public:
	static void serialize(ostream& output, unsigned char value) { serialize_int<unsigned char, 1>(output, value); }
	unsigned static char deserialize(istream& input) { return deserialize_int<unsigned char, 1>(input); }
};
template<>
class Serializer<bool>{
public:
	static void serialize(ostream& output, bool value) { serialize_int<bool, 1>(output, value); }
	static bool deserialize(istream& input) { return deserialize_int<bool, 1>(input); }
};

template<typename T>
class Serializer<vector<T>>{
public:
	static void serialize(ostream& output, const vector<T>& value){
		unsigned int length = value.size();
		serialize_value(output, length);
		
		for(T element: value){
			serialize_value(output, element);
		}
	}

	static vector<T> deserialize(istream& input){
		vector<T> result;
		
		auto size  = deserialize_value<unsigned int>(input);
		for(unsigned int i = 0; i < size; i++){
			result.push_back(deserialize_value<T>(input));
		}
		
		return result;
	}
};

template<>
class Serializer<string>{
public:
	static void serialize(ostream& output, const string& value){
		unsigned int length = value.size();
		serialize_value(output, length);
		output.write(value.data(), length);
	}

	static string deserialize(istream& input){
		auto length  = deserialize_value<unsigned int>(input);
		
		string result(length, '\0');
		input.read(&result[0], length);
		
		return result;
	}
};

#endif
