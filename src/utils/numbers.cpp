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

bool Number::operator<(Number other){
	return scaled_value < other.scaled_value;
}
bool Number::operator<=(Number other){
	return scaled_value <= other.scaled_value;
}
bool Number::operator>(Number other){
	return scaled_value > other.scaled_value;
}
bool Number::operator>=(Number other){
	return scaled_value >= other.scaled_value;
}
bool Number::operator==(Number other){
	return scaled_value == other.scaled_value;
}
bool Number::operator!=(Number other){
	return scaled_value != other.scaled_value;
}

bool Number::operator<(int other){
	return *this < Number(other);
}
bool Number::operator<=(int other){
	return *this <= Number(other);
}
bool Number::operator>(int other){
	return *this > Number(other);
}
bool Number::operator>=(int other){
	return *this >= Number(other);
}
bool Number::operator==(int other){
	return *this == Number(other);
}
bool Number::operator!=(int other){
	return *this != Number(other);
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


void Point::serialize(ostream& output) const{
	serialize_value(output, x);
	serialize_value(output, y);
}
Point Point::deserialize(istream& input){
	auto x = deserialize_value<Number>(input);
	auto y = deserialize_value<Number>(input);
	return {
		.x = x,
		.y = y
	};
}

Point Point::operator +(const Point& other) const{
	return {
		.x = x + other.x,
		.y = y + other.y
	};
}
Point Point::operator -(const Point& other) const{
	return {
		.x = x - other.x,
		.y = y - other.y
	};
}

Point& Point::operator +=(const Point& other){
	x += other.x;
	y += other.y;
	return *this;
}
Point& Point::operator -=(const Point& other){
	x -= other.x;
	y -= other.y;
	return *this;
}

Point Point::operator *(Number scale) const {
	return {
		.x = x * scale,
		.y = y * scale,
	};
}
Point Point::operator /(Number scale) const {
	return {
		.x = x / scale,
		.y = y / scale,
	};
}
Point& Point::operator *=(Number scale){
	x *= scale;
	y *= scale;
	return *this;
}
Point& Point::operator /=(Number scale){
	x /= scale;
	y /= scale;
	return *this;
}

Point Point::operator *(int scale) const {
	return {
		.x = x * scale,
		.y = y * scale,
	};
}
Point Point::operator /(int scale) const {
	return {
		.x = x / scale,
		.y = y / scale,
	};
}
Point& Point::operator *=(int scale){
	x *= scale;
	y *= scale;
	return *this;
}
Point& Point::operator /=(int scale){
	x /= scale;
	y /= scale;
	return *this;
}

Point Point::operator *(double scale) const {
	return {
		.x = x * scale,
		.y = y * scale,
	};
}
Point Point::operator /(double scale) const {
	return {
		.x = x / scale,
		.y = y / scale,
	};
}
Point& Point::operator *=(double scale){
	x *= scale;
	y *= scale;
	return *this;
}
Point& Point::operator /=(double scale){
	x /= scale;
	y /= scale;
	return *this;
}
