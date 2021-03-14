#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "utils.h"



char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}


char* uip_to_sip(unsigned ip) {
    char a = ip >> 24;
    char b = ip >> 16;
    char c = ip >> 8;
    char d = ip & 0xFF;

    char* p0 = malloc(sizeof(char)*3);
    char* p1 = malloc(sizeof(char)*3);
    char* p2 = malloc(sizeof(char)*3);
    char* p3 = malloc(sizeof(char)*3);

    itoa(a, p0, 10);
    itoa(b, p1, 10);
    itoa(c, p2, 10);
    itoa(d, p3, 10);

    char* result = strcat(p0, ".");
    result = strcat(result, p1);
    result = strcat(result, ".");
    result = strcat(result, p2);
    result = strcat(result, ".");
    result = strcat(result, p3);
    
    return result;
}

unsigned sip_to_uip(const char* ip) {
    char* ipc = malloc(sizeof(char)*strlen(ip));
    strcpy(ipc, ip);
    char* tmp = strtok(ipc, ".");
    char ipp[4];

    int i = 0;
    while(tmp != NULL) {
        ipp[i++] = atoi(tmp);
        tmp = strtok(NULL, ".");
        if(i>=5) {
            printf("sip_to_uip ERROR: string format error\n");
            exit(0);
        }
    }

    unsigned result = (ipp[0]*1u << 24) | (ipp[1]*1u << 16) | (ipp[2]*1u << 8) | (ipp[3] & 0xFF);
    return result;
}
