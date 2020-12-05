#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "depend-parser.h"

void depend_chain_init(struct depend_chain_s *chain)
{
    chain->serial_root = RB_ROOT;
}

static struct depend_chain_vertex_s *depend_chain_vertex_create_or_find(struct depend_chain_s *chain,
    const char *name)
{
    int ret;
    size_t len;
    struct rb_node **new;
    struct rb_node *parent;
    struct depend_chain_vertex_s *vertex;

    parent = NULL;
    new = &chain->serial_root.rb_node;
    while (*new) {
        parent = *new;
        vertex = rb_entry(parent, struct depend_chain_vertex_s, serial_rb);
        ret = strcmp(vertex->name, name);
        if (ret == 0) {
            return vertex;
        } else if (ret > 0) {
            new = &parent->rb_left;
        } else {
            new = &parent->rb_right;
        }
    }

    len = strlen(name) + 1;
    vertex = (struct depend_chain_vertex_s *)malloc(sizeof(struct depend_chain_vertex_s) + len);
    if (vertex == NULL) {
        return NULL;
    }

    vertex->depend_root = RB_ROOT;
    memcpy(vertex->name, name, len);
    rb_link_node(&vertex->serial_rb, parent, new);
    rb_insert_color(&vertex->serial_rb, &chain->serial_root);

    return vertex;
}

static struct depend_chain_vertex_depend_s *depend_chain_vertex_add_depend(struct depend_chain_s *chain,
    struct depend_chain_vertex_s *vertex, const char *name)
{
    int ret;
    struct rb_node **new;
    struct rb_node *parent;
    struct depend_chain_vertex_depend_s *dep;

    parent = NULL;
    new = &vertex->depend_root.rb_node;
    while (*new) {
        parent = *new;
        dep = rb_entry(parent, struct depend_chain_vertex_depend_s, rb);
        ret = strcmp(dep->vertex->name, name);
        if (ret == 0) {
            return dep;
        } else if (ret > 0) {
            new = &parent->rb_left;
        } else {
            new = &parent->rb_right;
        }
    }

    dep = (struct depend_chain_vertex_depend_s *)malloc(sizeof(struct depend_chain_vertex_depend_s));
    if (dep == NULL) {
        return NULL;
    }

    dep->vertex = depend_chain_vertex_create_or_find(chain, name);
    if (dep->vertex == NULL) {
        free(dep);
        return NULL;
    }

    rb_link_node(&dep->rb, parent, new);
    rb_insert_color(&dep->rb, &vertex->depend_root);

    return dep;
}

static void depend_chain_vertex_free_depend_root(struct depend_chain_vertex_s *vertex)
{
    struct rb_node *tmp;
    struct rb_node *node;
    struct rb_root *root;
    struct depend_chain_vertex_depend_s *dep;

    root = &vertex->depend_root;
    node = rb_first(root);
    while (node != NULL) {
        tmp = node;
        node = rb_next(node);
        rb_erase(tmp, root);
        dep = rb_entry(tmp, struct depend_chain_vertex_depend_s, rb);
        free(dep);
    }
}

void depend_chain_clean(struct depend_chain_s *chain)
{
    struct rb_node *tmp;
    struct rb_node *node;
    struct rb_root *root;
    struct depend_chain_vertex_s *vertex;

    if (chain == NULL) {
        return;
    }

    root = &chain->serial_root;
    node = rb_first(root);
    while (node != NULL) {
        tmp = node;
        node = rb_next(node);
        rb_erase(tmp, root);
        vertex = rb_entry(tmp, struct depend_chain_vertex_s, serial_rb);
        depend_chain_vertex_free_depend_root(vertex);
        free(vertex);
    }
}

int depend_chain_insert(struct depend_chain_s *chain, const char *name, const char *depend_list,
    const char *delim)
{
    char *tmp;
    char *mark;
    char *word;
    struct depend_chain_vertex_s *vertex;
    struct depend_chain_vertex_depend_s *dep;

    if (chain == NULL || name == NULL || *name == '\0' || (depend_list != NULL && delim == NULL)) {
        return -1;
    }

    if (depend_list == NULL) {
        depend_list = "";
    }

    tmp = strdup(depend_list);
    if (tmp == NULL) {
        return -1;
    }

    vertex = depend_chain_vertex_create_or_find(chain, name);
    if (vertex == NULL) {
        free(tmp);
        return -1;
    }

    word = tmp;
    mark = NULL;
    while ((word = strtok_r(word, delim, &mark)) != NULL) {
        dep = depend_chain_vertex_add_depend(chain, vertex, word);
        if (dep == NULL) {
            depend_chain_vertex_free_depend_root(vertex);
            rb_erase(&vertex->serial_rb, &chain->serial_root);
            free(vertex);
            free(tmp);
            return -1;
        }
        word = NULL;
    }
    free(tmp);

    return 0;
}
