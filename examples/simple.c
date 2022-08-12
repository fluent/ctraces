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

    /*
     * create an options context: this is used to initialize a CTrace context only,
     * it's not mandatory and you can pass a NULL instead on context creation.
     */
    ctr_opts_init(&opts);
    ctr_opts_set(&opts, CTR_OPTS_TRACE_ID, "my-trace-id-123456");

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

    /* add some attributes to the span */
    ctr_span_set_attribute_string(span_root, "agent", "Fluent Bit");
    ctr_span_set_attribute_int(span_root, "year", 2022);
    ctr_span_set_attribute_bool(span_root, "open_source", CTR_TRUE);

    /* add one event and set attributes to it */
    event = ctr_span_event_add(span_root, "something");
    ctr_span_event_set_attribute_string(event, "syscall", "open()");
    ctr_span_event_set_attribute_string(event, "syscall", "connect()");
    ctr_span_event_set_attribute_string(event, "syscall", "write()");


    /* create a child span */
    span_child = ctr_span_create(ctx, "do-work", span_root);
    if (!span_child) {
        ctr_destroy(ctx);
        ctr_opts_exit(&opts);
        exit(EXIT_FAILURE);
    }

    text = ctr_encode_text_create(ctx);
    printf("%s\n", text);
    ctr_encode_text_destroy(text);

    /* destroy the context */
    ctr_destroy(ctx);

    /* exit options (it release resources allocated) */
    ctr_opts_exit(&opts);

    return 0;
}
