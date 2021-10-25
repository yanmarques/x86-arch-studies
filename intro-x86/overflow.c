#include <stdio.h>
#include <string.h>

int hidden() {
    printf("called hidden .)!\n");
}

int main(int argc, char ** argv) {
    char other[200];
    strcpy(other, argv[1]);
    return 0;
}