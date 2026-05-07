#include <stdlib.h>

int main(void) {
	if (system("gcc -x c -g3 -O0 -DXENOTATION_MAIN -DXENOTATION_IMPLEMENTATION src/xenotation.h -o xenotation -lgmp")) return 0;
	return 1;
}