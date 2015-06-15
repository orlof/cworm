//
// Created by Teppo Koskinen on 15/06/15.
//

#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

void recover(char *msg) {
    printf("ERROR: %s", msg);
    exit(-1);
}
