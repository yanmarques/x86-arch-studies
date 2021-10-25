#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

int main() {
    printf("%lu", sizeof(struct in_addr));
}