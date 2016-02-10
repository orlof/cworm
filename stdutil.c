#include "memory.h"
#include "stdutil.h"
#include "array.h"
#include "dict.h"
#include "integer.h"
#include "float.h"
#include "string.h"
#include "interpreter.h"

static REF eval_name(REF ref);
static REF eval_reference(REF ref);
static REF eval_subscription(REF ref);
static REF eval_tuple(REF ref);

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

    if(IS_TYPE_FLOAT(leftflg) && leftflg == rightflg) {
        return float_cmp(leftflg, rightflg);
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

REF eval(REF orig) {
    Handle *h = &HANDLES.slot[orig];
    switch(h->flags) {
        case NAME:
            return eval_name(orig);
        case REFERENCE:
            return eval_reference(orig);
        case SUBSCRIPTION:
            return eval_subscription(orig);
        case TUPLE_COMMA:
            h->flags = TUPLE;
        case TUPLE:
            return eval_tuple(orig);
    }
    return orig;
}

REF eval_name(REF ref) {
    REF val = scope_get(current_scope, ref);
    if(val == ILLEGAL_REF) {
        recover("Unknown ident\n");
    }
    return val;
}

REF eval_reference(REF ref) {

}

REF eval_subscription(REF ref) {

}

REF eval_tuple(REF ref) {

}

REF scope_get(REF scope, REF ref) {
    REF val = dict_get(scope, ref);

    if(val != ILLEGAL_REF) {
        return val;
    }

    REF parent = dict_get(scope, SCOPE_PARENT);
    if(parent != ILLEGAL_REF) {
        return scope_get(parent, ref);
    }

    return ILLEGAL_REF;
}