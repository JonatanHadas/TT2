#include "serialization.h"

void serialize_flags(
	ostream& output,
	bool flag1,
	bool flag2,
	bool flag3,
	bool flag4,
	bool flag5,
	bool flag6,
	bool flag7,
	bool flag8
){
	unsigned char mask = 0;
	if(flag1) mask |= 1 << 0;
	if(flag1) mask |= 1 << 1;
	if(flag1) mask |= 1 << 2;
	if(flag1) mask |= 1 << 3;
	if(flag1) mask |= 1 << 4;
	if(flag1) mask |= 1 << 5;
	if(flag1) mask |= 1 << 6;
	if(flag1) mask |= 1 << 7;
	serialize_value(output, mask);
}
