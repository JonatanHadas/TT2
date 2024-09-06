#include "numbers.h"

#include "serialization.h"

#include <math.h>

#define SCALE (1 << 16)

Number::Number(int scaled_value, int) : scaled_value(scaled_value) {}

Number::Number(int value) : scaled_value(SCALE * value) {}
Number::Number(double value) : scaled_value(round(SCALE * value)) {}

Number Number::operator -() const {
	return Number(-scaled_value, 0);
}

Number Number::operator +(Number other) const{
	return Number(scaled_value + other.scaled_value, 0);
}
Number Number::operator -(Number other) const{
	return Number(scaled_value - other.scaled_value, 0);
}
Number Number::operator *(Number other) const{
	return Number(((long long int)scaled_value * (long long int)other.scaled_value) / SCALE, 0);
}
Number Number::operator /(Number other) const{
	long long int scaled_value1 = scaled_value, scaled_value2 = other.scaled_value;
	return Number(((long long int)scaled_value * SCALE) / other.scaled_value, 0);
}

Number& Number::operator +=(Number other){
	scaled_value += other.scaled_value;
	return *this;
}
Number& Number::operator -=(Number other){
	scaled_value -= other.scaled_value;
	return *this;
}
Number& Number::operator *=(Number other){
	scaled_value = ((long long int)scaled_value * (long long int)other.scaled_value) / SCALE;
	return *this;
}
Number& Number::operator /=(Number other){
	scaled_value = ((long long int)scaled_value * SCALE) / other.scaled_value;
	return *this;
}

Number Number::operator +(int other) const{
	return Number(scaled_value + (SCALE * other), 0);
}
Number Number::operator -(int other) const{
	return Number(scaled_value - (SCALE * other), 0);
}
Number Number::operator *(int other) const{
	return Number(scaled_value * other, 0);
}
Number Number::operator /(int other) const{
	return Number(scaled_value / other, 0);
}

Number& Number::operator +=(int other){
	scaled_value += SCALE * other;
	return *this;
}
Number& Number::operator -=(int other){
	scaled_value -= SCALE * other;
	return *this;
}
Number& Number::operator *=(int other){
	scaled_value *= other;
	return *this;
}
Number& Number::operator /=(int other){
	scaled_value /= other;
	return *this;
}

Number Number::operator +(double other) const{
	return *this + Number(other);
}
Number Number::operator -(double other) const{
	return *this - Number(other);
}
Number Number::operator *(double other) const{
	return Number(scaled_value * other, 0);
}
Number Number::operator /(double other) const{
	return Number(scaled_value / other, 0);
}

Number& Number::operator +=(double other){
	return *this += Number(other);
}
Number& Number::operator -=(double other){
	return *this -= Number(other);
}
Number& Number::operator *=(double other){
	scaled_value *= other;
	return *this;
}
Number& Number::operator /=(double other){
	scaled_value /= other;
	return *this;
}

Number operator+(int num1, Number num2){
	return Number((num1 * SCALE) + num2.scaled_value, 0);
}
Number operator-(int num1, Number num2){
	return Number((num1 * SCALE) - num2.scaled_value, 0);
}
Number operator*(int num1, Number num2){
	return Number(num1 * num2.scaled_value, 0);
}
Number operator/(int num1, Number num2){
	return Number(((long long int)num1 * SCALE * SCALE)/num2.scaled_value, 0);
}

Number operator+(double num1, Number num2){
	return Number(num1) + num2;
}
Number operator-(double num1, Number num2){
	return Number(num1) - num2;
}
Number operator*(double num1, Number num2){
	return Number(num1 * num2.scaled_value, 0);
}
Number operator/(double num1, Number num2){
	return Number((num1 * SCALE * SCALE)/num2.scaled_value, 0);
}

Number::operator int() const{
	return scaled_value / SCALE;
}
Number::operator double() const{
	return ((double) scaled_value) / SCALE;
}

void Number::serialize(ostream& output) const{
	serialize_value(output, scaled_value);
}
Number Number::deserialize(istream& input){
	return Number(deserialize_value<int>(input), 0);
}
