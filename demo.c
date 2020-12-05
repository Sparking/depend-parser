#include <stdio.h>
#include <stdlib.h>
#include "depend-parser.h"

int main(void)
{
    int ret;
    struct depend_chain_s chain;

    depend_chain_init(&chain);

    ret = depend_chain_insert(&chain, "hello", "a b c dsdf asdfwe asdfas", " ");
    if (ret != 0) {
        fprintf(stderr, "insert failed!\n");
        return -1;
    }

    ret = depend_chain_insert(&chain, "hello", "a b c dsdf asdfwe asdfas sadfasdf", " ");
    if (ret != 0) {
        fprintf(stderr, "insert failed!\n");
        return -1;
    }

    ret = depend_chain_insert(&chain, "asdfwe", "a b c dsdf asdfas", " ");
    if (ret != 0) {
        fprintf(stderr, "insert failed!\n");
        return -1;
    }

    depend_chain_clean(&chain);

    return 0;
}
