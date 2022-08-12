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

static void format_span(cfl_sds_t *buf, struct ctrace *ctx, struct ctrace_span *span, int level)
{
    int min;
    int off = 1 + (level * 4);
    char tmp[1024];
    struct ctrace_span *s;
    struct cfl_list *head;

    min = off + 4;

    sds_cat_safe(buf, "\n");

    snprintf(tmp, sizeof(tmp) - 1, "%*s[span '%s']\n", off, "", span->name);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- kind      : %s\n", min, "", ctr_span_kind_string(span));
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- start_time: %" PRIu64 "\n", min, "", span->start_time);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- end_time  : %" PRIu64 "\n", min, "", span->end_time);
    sds_cat_safe(buf, tmp);

    snprintf(tmp, sizeof(tmp) - 1, "%*s- attributes: []\n", min, "");
    sds_cat_safe(buf, tmp);


    /* iterate child spans */
    cfl_list_foreach(head, &span->childs) {
        s = cfl_list_entry(head, struct ctrace_span, _head);
        format_span(buf, ctx, s, ++level);
    }
}

cfl_sds_t ctr_encode_text_create(struct ctrace *ctx)
{
    int span_levels = 0;
    cfl_sds_t buf;
    cfl_sds_t tmp;
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
