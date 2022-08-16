/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  CTraces
 *  =======
 *  Copyright 2022 Eduardo Silva <eduardo@calyptia.com>
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

static inline void sds_cat_safe(cfl_sds_t *buf, char *str)
{
    int len;

    len = strlen(str);
    cfl_sds_cat_safe(buf, str, len);
}

static void format_string(cfl_sds_t *buf, cfl_sds_t val, int level)
{
    char tmp[1024];

    snprintf(tmp, sizeof(tmp) - 1, "'%s'", val);
    sds_cat_safe(buf, tmp);
}

static void format_int64(cfl_sds_t *buf, int64_t val, int level)
{
    char tmp[1024];

    snprintf(tmp, sizeof(tmp) - 1, "%" PRIi64, val);
    sds_cat_safe(buf, tmp);
}

static void format_double(cfl_sds_t *buf, double val, int level)
{
    char tmp[1024];

    snprintf(tmp, sizeof(tmp) - 1, "%.17g", val);
    sds_cat_safe(buf, tmp);
}

static void format_bool(cfl_sds_t *buf, int val, int level)
{
    if (val) {
        sds_cat_safe(buf, "true");
    }
    else {
        sds_cat_safe(buf, "false");
    }
}

static void format_array(cfl_sds_t *buf, struct cfl_array *array, int level)
{
    int i;
    int off = level + 4;
    char tmp[128];
    struct cfl_variant *v;

    sds_cat_safe(buf, "[\n");

    snprintf(tmp, sizeof(tmp) - 1, "%*s", off, "");

    for (i = 0; i < array->entry_count; i++) {
        v = array->entries[i];

        sds_cat_safe(buf, tmp);

        if (v->type == CFL_VARIANT_STRING) {
            format_string(buf, v->data.as_string, off);
        }
        else if (v->type == CFL_VARIANT_BOOL) {
            format_bool(buf, v->data.as_bool, off);
        }
        else if (v->type == CFL_VARIANT_INT) {
            format_int64(buf, v->data.as_int64, off);
        }
        else if (v->type == CFL_VARIANT_DOUBLE) {
            format_double(buf, v->data.as_double, off);
        }
        else if (v->type == CFL_VARIANT_ARRAY) {
            format_array(buf, v->data.as_array, off);
        }

        if (i + 1 < array->entry_count) {
            sds_cat_safe(buf, ",\n");
        }
    }

    off = level;
    snprintf(tmp, sizeof(tmp) - 1, "\n%*s]", off, "");
    sds_cat_safe(buf, tmp);
}

static void format_attributes(cfl_sds_t *buf, struct cfl_kvlist *kv, int level)
{
    int off = level + 4;
    char tmp[1024];
    struct cfl_list *head;
    struct cfl_kvpair *p;
    struct cfl_variant *v;

    sds_cat_safe(buf, "\n");

    cfl_list_foreach(head, &kv->list) {
        p = cfl_list_entry(head, struct cfl_kvpair, _head);

        /* key */
        snprintf(tmp, sizeof(tmp) - 1, "%*s- %s: ", off, "", p->key);
        sds_cat_safe(buf, tmp);

        /* value */
        v = p->val;
        if (v->type == CFL_VARIANT_STRING) {
            format_string(buf, v->data.as_string, off);
        }
        else if (v->type == CFL_VARIANT_BOOL) {
            format_bool(buf, v->data.as_bool, off);
        }
        else if (v->type == CFL_VARIANT_INT) {
            format_int64(buf, v->data.as_int64, off);
        }
        else if (v->type == CFL_VARIANT_DOUBLE) {
            format_double(buf, v->data.as_double, off);
        }
        else if (v->type == CFL_VARIANT_ARRAY) {
            format_array(buf, v->data.as_array, off);
        }
        else if (v->type == CFL_VARIANT_KVLIST) {
            format_attributes(buf, v->data.as_kv, off);
        }

        sds_cat_safe(buf, "\n");
    }
}

static void format_event(cfl_sds_t *buf, struct ctrace_span_event *event, int level)
{
    int off = level + 4;
    char tmp[1024];

    sds_cat_safe(buf, "\n");

    snprintf(tmp, sizeof(tmp) - 1, "%*s- name: %s\n", off, "", event->name);
    sds_cat_safe(buf, tmp);
    off += 4;

    snprintf(tmp, sizeof(tmp) - 1, "%*s- timestamp               : %" PRIu64 "\n", off, "", event->timestamp);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- dropped_attributes_count: %" PRIu32 "\n", off, "", event->dropped_attr_count);
    sds_cat_safe(buf, tmp);

    if (ctr_attributes_count(event->attr) > 0) {

        snprintf(tmp, sizeof(tmp) - 1, "%*s- attributes:", off, "");
        sds_cat_safe(buf, tmp);

        format_attributes(buf, event->attr->kv, off);
    }
    else {
        snprintf(tmp, sizeof(tmp) - 1, "%*s- attributes: none\n", off, "");
        sds_cat_safe(buf, tmp);
    }

}

static void format_span(cfl_sds_t *buf, struct ctrace *ctx, struct ctrace_span *span, int level)
{
    int min;
    int off = 1 + (level * 4);
    char tmp[1024];
    struct ctrace_span *s;
    struct ctrace_span_event *event;
    struct cfl_list *head;

    min = off + 4;

    snprintf(tmp, sizeof(tmp) - 1, "%*s[span '%s']\n", off, "", span->name);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- kind                    : %i (%s)\n", min, "", span->kind, ctr_span_kind_string(span));
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- start_time              : %" PRIu64 "\n", min, "", span->start_time);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- end_time                : %" PRIu64 "\n", min, "", span->end_time);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- dropped_attributes_count: %" PRIu32 "\n", min, "", span->dropped_attr_count);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- dropped_events_count    : %" PRIu32 "\n", min, "", span->dropped_events_count);
    sds_cat_safe(buf, tmp);

    /* Status */
    snprintf(tmp, sizeof(tmp) - 1, "%*s- status:\n", min, "");
    sds_cat_safe(buf, tmp);
    snprintf(tmp, sizeof(tmp) - 1, "%*s- code        : %i\n", min + 4, "", span->status.code);
    sds_cat_safe(buf, tmp);

    if (span->status.message) {
        snprintf(tmp, sizeof(tmp) - 1, "%*s- message : '%s'\n", min + 4, "", span->status.message);
    }

    /* span attributes */
    if (ctr_attributes_count(span->attr) == 0) {
        snprintf(tmp, sizeof(tmp) - 1, "%*s- attributes: none\n", min, "");
        sds_cat_safe(buf, tmp);
    }
    else {
        snprintf(tmp, sizeof(tmp) - 1, "%*s- attributes: ", min, "");
        sds_cat_safe(buf, tmp);
        format_attributes(buf, span->attr->kv, min);
    }

    /* events */
    if (cfl_list_size(&span->events) == 0) {
        snprintf(tmp, sizeof(tmp) - 1, "%*s- events: none\n", min, "");
        sds_cat_safe(buf, tmp);
    }
    else {
        snprintf(tmp, sizeof(tmp) - 1, "%*s- events: ", min, "");
        sds_cat_safe(buf, tmp);

        cfl_list_foreach(head, &span->events) {
            event = cfl_list_entry(head, struct ctrace_span_event, _head);
            format_event(buf, event, min);
        }
    }

    /* iterate child spans */
    cfl_list_foreach(head, &span->childs) {
        s = cfl_list_entry(head, struct ctrace_span, _head);
        format_span(buf, ctx, s, ++level);
    }
}

cfl_sds_t ctr_encode_text_create(struct ctrace *ctx)
{
    cfl_sds_t buf;
    struct cfl_list *head;
    struct ctrace_span *span;

    buf = cfl_sds_create_size(1024);
    if (!buf) {
        return NULL;
    }

    /* trace id */
    sds_cat_safe(&buf, "|--------- trace-id: ");

    if (ctx->trace_id) {
        sds_cat_safe(&buf, ctx->trace_id);
    }
    else {
        sds_cat_safe(&buf, "--");
    }
    sds_cat_safe(&buf, " ---------|\n");

    /* iterate spans */
    cfl_list_foreach(head, &ctx->spans) {
        span = cfl_list_entry(head, struct ctrace_span, _head);
        format_span(&buf, ctx, span, 0);
    }

    return buf;
}

void ctr_encode_text_destroy(cfl_sds_t text)
{
    cfl_sds_destroy(text);
}
