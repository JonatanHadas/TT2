#include "utils.h"
#include <stdlib.h>

int rand_range(int min, int max){
	return rand()%(max-min) + min;
}
