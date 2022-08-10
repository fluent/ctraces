/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <ctraces/ctraces.h>

int main()
{
    struct ctrace *ctx;
    struct ctrace_opts opts;
    struct ctrace_span *span_root;

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

    /* Create a span (root span) */
    span_root = ctr_span_create(ctx, NULL);
    if (!span_root) {
	    ctr_destroy(ctx);
    	ctr_opts_exit(&opts);
    	exit(EXIT_FAILURE);
    }

    /* add some attributes to the span */
    ctr_span_set_attribute_string(span_root, "agent", "Fluent Bit");
    ctr_span_set_attribute_int(span_root, "year", 2022);
    ctr_span_set_attribute_bool(span_root, "open_source", CTR_TRUE);

    /* destroy the context */
    ctr_destroy(ctx);

    /* exit options (it release resources allocated) */
    ctr_opts_exit(&opts);

    return 0;
}
