/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <ctraces/ctraces.h>

int main()
{
    char *text;
    struct ctrace *ctx;
    struct ctrace_opts opts;
    struct ctrace_span *span_root;
    struct ctrace_span *span_child;
    struct ctrace_span_event *event;
    struct ctrace_resource *res;
    struct ctrace_id *id;
    struct cfl_array *array;
    struct cfl_array *sub_array;
    struct cfl_kvlist *kv;

    /*
     * create an options context: this is used to initialize a CTrace context only,
     * it's not mandatory and you can pass a NULL instead on context creation.
     */
    ctr_opts_init(&opts);
    ctr_opts_set(&opts, CTR_OPTS_TRACE_ID, "abd6253728372639");

    /* ctrace context */
    ctx = ctr_create(&opts);
    if (!ctx) {
        ctr_opts_exit(&opts);
        exit(EXIT_FAILURE);
    }

    /* Create a root span */
    span_root = ctr_span_create(ctx, "main", NULL);
    if (!span_root) {
        ctr_destroy(ctx);
        ctr_opts_exit(&opts);
        exit(EXIT_FAILURE);
    }

    /* Create some detault resource, populated already with some attributes */
    res = ctr_resource_create_default(ctx);
    ctr_span_set_resource(span_root, res);

    /* Set a random ID to the span */
    id = ctr_id_create_random();
    ctr_span_set_id_with_cid(span_root, id);

    /* add some attributes to the span */
    ctr_span_set_attribute_string(span_root, "agent", "Fluent Bit");
    ctr_span_set_attribute_int64(span_root, "year", 2022);
    ctr_span_set_attribute_bool(span_root, "open_source", CTR_TRUE);
    ctr_span_set_attribute_double(span_root, "temperature", 25.5);

    /* pack an array: create an array context by using the CFL api */
    array = cfl_array_create(4);
    cfl_array_append_string(array, "first");
    cfl_array_append_double(array, 2.0);
    cfl_array_append_bool(array, CFL_FALSE);

    sub_array = cfl_array_create(3);
    cfl_array_append_double(sub_array, 3.1);
    cfl_array_append_double(sub_array, 5.2);
    cfl_array_append_double(sub_array, 6.3);
    cfl_array_append_array(array, sub_array);

    /* add array to the attribute list */
    ctr_span_set_attribute_array(span_root, "my_array", array);

    /* event: add one event and set attributes to it */
    event = ctr_span_event_add(span_root, "connect to remote server");

    ctr_span_event_set_attribute_string(event, "syscall 1", "open()");
    ctr_span_event_set_attribute_string(event, "syscall 2", "connect()");
    ctr_span_event_set_attribute_string(event, "syscall 3", "write()");

    /* add a key/value pair list */
    kv = cfl_kvlist_create(1);
    cfl_kvlist_insert_string(kv, "language", "c");

    ctr_span_set_attribute_kvlist(span_root, "my-list", kv);

    /* create a child span */
    span_child = ctr_span_create(ctx, "do-work", span_root);
    if (!span_child) {
        ctr_destroy(ctx);
        ctr_opts_exit(&opts);
        exit(EXIT_FAILURE);
    }

    /* use same resource information as the first span */
    ctr_span_set_resource(span_child, res);

    /* we will set the span ID as empty, but use the random ID generated as parent span id */
    ctr_span_set_parent_id_with_cid(span_child, id);

    /* destroy the id since is not longer needed */
    ctr_id_destroy(id);

    /* change span kind to client */
    ctr_span_kind_set(span_child, CTRACE_SPAN_CLIENT);

    /* Encode Trace as a readable text */
    text = ctr_encode_text_create(ctx);
    printf("%s\n", text);
    ctr_encode_text_destroy(text);

    /* destroy the context */
    ctr_destroy(ctx);

    /* exit options (it release resources allocated) */
    ctr_opts_exit(&opts);

    return 0;
}
