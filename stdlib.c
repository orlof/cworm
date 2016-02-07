#include "memory.h"
#include "stdlib.h"

int val_cmp(REF leftref, REF rightref) {
    if(leftref == rightref) {
        // shortcut for same object
        return 0;
    }

    Handle *lefthdl = &HANDLES.slot[leftref];
    Handle *righthdl = &HANDLES.slot[rightref];

    FLAGS leftflg = lefthdl->flags;
    FLAGS rightflg = righthdl->flags;

    if(IS_TYPE_INTEGER(leftflg) && IS_TYPE_INTEGER(rightflg)) {
        return int_cmp(leftref, rightref);
    }

    if(IS_TYPE_FLOAT(leftflg) && IS_TYPE_INTEGER(rightflg)) {
        return float_cmp_int(leftflg, rightflg);
    }

    if(IS_TYPE_INTEGER(leftflg) && IS_TYPE_FLOAT(rightflg)) {
        return -float_cmp_int(rightflg, leftflg);
    }

    if(IS_TYPE_LITERAL(leftflg) && leftflg == rightflg) {
        return str_cmp(leftref, rightref);
    }

    if(IS_TYPE_ARRAY(leftflg) && leftflg == rightflg) {
        return array_cmp(leftref, rightref);
    }

    if(leftflg < rightflg)
        return -1;
    else if(leftflg > rightflg)
        return 1;
    else
        return 0;
}
