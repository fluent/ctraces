/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  CTraces
 *  =======
 *  Copyright 2022 The CTraces Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <ctraces/ctraces.h>
#include <ctraces/ctr_encode_opentelemetry.h>
#include <ctraces/ctr_decode_opentelemetry.h>
#include "ctr_tests.h"

/* build a non-trivial trace that exercises spans, events, links, status,
 * attributes (scalars/array/kvlist) and an instrumentation scope - the same
 * surface that triggered the encoder NULL-deref fixes.
 */
static struct ctrace *build_sample_trace()
{
    struct ctrace *ctx;
    struct ctrace_span *span_root;
    struct ctrace_span *span_child;
    struct ctrace_span_event *event;
    struct ctrace_resource_span *resource_span;
    struct ctrace_resource *resource;
    struct ctrace_scope_span *scope_span;
    struct ctrace_instrumentation_scope *scope;
    struct ctrace_link *link;
    struct ctrace_id *span_id;
    struct ctrace_id *trace_id;
    struct cfl_array *array;
    struct cfl_kvlist *kv;

    ctx = ctr_create(NULL);
    if (!ctx) {
        return NULL;
    }

    resource_span = ctr_resource_span_create(ctx);
    ctr_resource_span_set_schema_url(resource_span, "https://ctraces/rs");

    resource = ctr_resource_span_get_resource(resource_span);
    ctr_resource_set_dropped_attr_count(resource, 5);

    scope_span = ctr_scope_span_create(resource_span);
    ctr_scope_span_set_schema_url(scope_span, "https://ctraces/ss");

    scope = ctr_instrumentation_scope_create("ctrace", "1.2.3", 3, NULL);
    ctr_scope_span_set_instrumentation_scope(scope_span, scope);

    trace_id = ctr_id_create_random(CTR_ID_OTEL_TRACE_SIZE);
    span_id = ctr_id_create_random(CTR_ID_OTEL_SPAN_SIZE);

    span_root = ctr_span_create(ctx, scope_span, "main", NULL);
    ctr_span_set_trace_id_with_cid(span_root, trace_id);
    ctr_span_set_span_id_with_cid(span_root, span_id);
    ctr_span_set_status(span_root, CTRACE_SPAN_STATUS_CODE_OK, "all good");

    ctr_span_set_attribute_string(span_root, "service.name", "ctraces");
    ctr_span_set_attribute_int64(span_root, "year", 2026);
    ctr_span_set_attribute_bool(span_root, "open_source", CTR_TRUE);
    ctr_span_set_attribute_double(span_root, "ratio", 0.42);

    array = cfl_array_create(3);
    cfl_array_append_string(array, "a");
    cfl_array_append_int64(array, 1);
    cfl_array_append_bool(array, CFL_FALSE);
    ctr_span_set_attribute_array(span_root, "list", array);

    kv = cfl_kvlist_create();
    cfl_kvlist_insert_string(kv, "language", "c");
    ctr_span_set_attribute_kvlist(span_root, "meta", kv);

    event = ctr_span_event_add(span_root, "connect");
    ctr_span_event_set_attribute_string(event, "host", "localhost");
    ctr_span_event_set_dropped_attributes_count(event, 1);

    span_child = ctr_span_create(ctx, scope_span, "do-work", span_root);
    ctr_span_set_trace_id_with_cid(span_child, trace_id);
    ctr_span_kind_set(span_child, CTRACE_SPAN_CLIENT);

    link = ctr_link_create_with_cid(span_child, trace_id, span_id);
    ctr_link_set_trace_state(link, "state=1");
    ctr_link_set_dropped_attr_count(link, 2);

    ctr_id_destroy(trace_id);
    ctr_id_destroy(span_id);

    return ctx;
}

/* count spans across the whole context, traversing the resource/scope tree
 * so we verify that the decoder rebuilt the structure end-to-end.
 */
static int count_spans(struct ctrace *ctx)
{
    int count = 0;
    struct cfl_list *rs_head;
    struct cfl_list *ss_head;
    struct ctrace_resource_span *rs;
    struct ctrace_scope_span *ss;

    cfl_list_foreach(rs_head, &ctx->resource_spans) {
        rs = cfl_list_entry(rs_head, struct ctrace_resource_span, _head);
        cfl_list_foreach(ss_head, &rs->scope_spans) {
            ss = cfl_list_entry(ss_head, struct ctrace_scope_span, _head);
            count += cfl_list_size(&ss->spans);
        }
    }

    return count;
}

void test_otlp_roundtrip()
{
    cfl_sds_t buf;
    size_t offset = 0;
    struct ctrace *ctx;
    struct ctrace *decoded;
    int ret;

    ctx = build_sample_trace();
    TEST_ASSERT(ctx != NULL);

    buf = ctr_encode_opentelemetry_create(ctx);
    TEST_ASSERT(buf != NULL);
    TEST_CHECK(cfl_sds_len(buf) > 0);

    ret = ctr_decode_opentelemetry_create(&decoded, buf, cfl_sds_len(buf), &offset);
    TEST_ASSERT(ret == CTR_DECODE_OPENTELEMETRY_SUCCESS);
    TEST_ASSERT(decoded != NULL);
    TEST_CHECK(count_spans(decoded) == 2);

    ctr_encode_opentelemetry_destroy(buf);
    ctr_destroy(decoded);
    ctr_destroy(ctx);
}

void test_otlp_decode_corrupted()
{
    struct ctrace *decoded = NULL;
    size_t offset = 0;
    /* not a valid protobuf payload */
    char garbage[] = "\xff\xff\xff\xff\xff\xff\xff\xff";
    int ret;

    ret = ctr_decode_opentelemetry_create(&decoded, garbage, sizeof(garbage) - 1, &offset);
    TEST_CHECK(ret != CTR_DECODE_OPENTELEMETRY_SUCCESS);

    /* offset past end of buffer must be reported as insufficient data,
     * not crash.
     */
    offset = 64;
    ret = ctr_decode_opentelemetry_create(&decoded, garbage, sizeof(garbage) - 1, &offset);
    TEST_CHECK(ret == CTR_DECODE_OPENTELEMETRY_INSUFFICIENT_DATA);
}

TEST_LIST = {
    {"otlp_roundtrip",       test_otlp_roundtrip},
    {"otlp_decode_corrupted", test_otlp_decode_corrupted},
    { 0 }
};
