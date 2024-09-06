#ifndef _NUMBERS_H
#define _NUMBERS_H

#include <iostream>

using namespace std;

class Number;

Number operator+(int num1, Number num2);
Number operator-(int num1, Number num2);
Number operator*(int num1, Number num2);
Number operator/(int num1, Number num2);

Number operator+(double num1, Number num2);
Number operator-(double num1, Number num2);
Number operator*(double num1, Number num2);
Number operator/(double num1, Number num2);

// Fixed point numbers
class Number{
	int scaled_value;
	
	explicit Number(int scaled_value, int);

	friend Number operator+(int num1, Number num2);
	friend Number operator-(int num1, Number num2);
	friend Number operator*(int num1, Number num2);
	friend Number operator/(int num1, Number num2);

	friend Number operator+(double num1, Number num2);
	friend Number operator-(double num1, Number num2);
	friend Number operator*(double num1, Number num2);
	friend Number operator/(double num1, Number num2);
public:
	Number(int value);
	Number(double value);
	
	Number operator -() const;

	Number operator +(Number other) const;
	Number operator -(Number other) const;
	Number operator *(Number other) const;
	Number operator /(Number other) const;

	Number& operator +=(Number other);
	Number& operator -=(Number other);
	Number& operator *=(Number other);
	Number& operator /=(Number other);

	Number operator +(int other) const;
	Number operator -(int other) const;
	Number operator *(int other) const;
	Number operator /(int other) const;

	Number& operator +=(int other);
	Number& operator -=(int other);
	Number& operator *=(int other);
	Number& operator /=(int other);

	Number operator +(double other) const;
	Number operator -(double other) const;
	Number operator *(double other) const;
	Number operator /(double other) const;

	Number& operator +=(double other);
	Number& operator -=(double other);
	Number& operator *=(double other);
	Number& operator /=(double other);
	
	operator int() const;
	operator double() const;
	
	void serialize(ostream& output) const;
	static Number deserialize(istream& input);
};

#endif
