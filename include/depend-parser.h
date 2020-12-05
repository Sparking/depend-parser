#ifndef _DEPEND_PARSER_H_
#define _DEPEND_PARSER_H_

#include "list.h"
#include "rbtree.h"

struct depend_chain_vertex_s {
    struct rb_node serial_rb;
    struct rb_root depend_root;
    char name[0];
};

struct depend_chain_vertex_depend_s {
    struct rb_node rb;
    struct depend_chain_vertex_s *vertex;
};

struct depend_chain_s {
    struct rb_root serial_root;
};

extern void depend_chain_init(struct depend_chain_s *chain);

extern void depend_chain_clean(struct depend_chain_s *chain);

extern int depend_chain_insert(struct depend_chain_s *chain, const char *name, const char *depend_list,
    const char *delim);

#endif /* _EVENT_LOOP_H_ */
