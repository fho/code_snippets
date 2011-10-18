#include <stdlib.h>
#include <stdio.h>

void print_bytes(void* buf, size_t len){
    for(unsigned int i = 0;i < len; i++){
        if(len != 0)
            printf(":");
        printf("%02x", *((uint8_t*) ((char*) buf + i)));
    }
    printf("\n");
}
