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

void ctr_opts_init(struct ctrace_opts *opts)
{
    memset(opts, '\0', sizeof(struct ctrace_opts));
}

void ctr_opts_set(struct ctrace_opts *opts, int value, char *val)
{
    if (value == CTR_OPTS_TRACE_ID) {
        opts->trace_id = cfl_sds_create(val);
    }
}

void ctr_opts_exit(struct ctrace_opts *opts)
{

    if (!opts) {
        return;
    }

    if (opts->trace_id) {
        cfl_sds_destroy(opts->trace_id);
    }
}

/* Create a CTrace context */
struct ctrace *ctr_create(struct ctrace_opts *opts)
{
    struct ctrace *ctx;

    ctx = calloc(1, sizeof(struct ctrace));
    if (!ctx) {
        ctr_errno();
        return NULL;
    }
    cfl_list_init(&ctx->spans);

    if (opts) {
        if (opts->trace_id) {
            ctx->trace_id = cfl_sds_create(opts->trace_id);
        }
    }


    return ctx;
}

void ctr_destroy(struct ctrace *ctx)
{
    struct cfl_list *head;
    struct cfl_list *tmp;
    struct ctrace_span *span;

    if (ctx->trace_id) {
        cfl_sds_destroy(ctx->trace_id);
    }

    cfl_list_foreach_safe(head, tmp, &ctx->spans) {
        span = cfl_list_entry(head, struct ctrace_span, _head);
        ctr_span_destroy(span);
    }

    free(ctx);
}

