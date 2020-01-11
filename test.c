#include <stdio.h>
#include <string.h>
#include <stdint.h>

int shiftTable[] = {1, 2, 3, 4, 5, 6, 7};


int main(int argc, char* argv[]) {
    uint8_t lenght = sizeof(shiftTable)/sizeof(shiftTable[0]);
    printf("Size of massive: %d\n", lenght);
    for(uint8_t i = 0; i < lenght; i++) {
        printf("%d\n", shiftTable[i]);
    }
}
