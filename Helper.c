//
// Created by Hasee on 2019/7/30.
//

#include "include/Helper.h"


void reverBits(unsigned char *cp, unsigned int len){
    if(len < 2) return;

    int hl = len/2;

    while (hl < len){
        unsigned char c = cp[hl];
        cp[hl] = cp[len-1-hl];
        cp[len-1-hl] = c;
        hl++;
    }
}

int decodeUnsignedLeb128(u1 *bs, unsigned int *len) {

    u1 *ptr = bs;
    int result = *(bs++);

    if (result > 0x7f) {
        int cur = *(bs++);
        result = (result & 0x7f) | ((cur & 0x7f) << 7);
        if (cur > 0x7f) {
            cur = *(bs++);
            result |= (cur & 0x7f) << 14;
            if (cur > 0x7f) {
                cur = *(bs++);
                result |= (cur & 0x7f) << 21;
                if (cur > 0x7f) {
                    cur = *(bs++);
                    result |= cur << 28;
                }
            }
        }
    }

    *len = (bs - ptr);

    return result;

}

int decodeSignedLeb128(u1 *bs, unsigned int *len) {

    u1* ptr = bs;
    int result = *(bs++);

    if (result <= 0x7f) {
        result = (result << 25) >> 25;
    } else {
        int cur = *(bs++);
        result = (result & 0x7f) | ((cur & 0x7f) << 7);
        if (cur <= 0x7f) {
            result = (result << 18) >> 18;
        } else {
            cur = *(bs++);
            result |= (cur & 0x7f) << 14;
            if (cur <= 0x7f) {
                result = (result << 11) >> 11;
            } else {
                cur = *(bs++);
                result |= (cur & 0x7f) << 21;
                if (cur <= 0x7f) {
                    result = (result << 4) >> 4;
                } else {
                    cur = *(bs++);
                    result |= cur << 28;
                }
            }
        }
    }

    *len = bs -ptr;

    return result;

}




