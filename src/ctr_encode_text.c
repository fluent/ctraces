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

cfl_sds_t ctr_encode_text_create(struct ctrace *ctx)
{
    int span_levels = 0;
    cfl_sds_t buf;
    cfl_sds_t tmp;
    struct cfl_list *head;

    buf = cfl_sds_create_size(1024);
    if (!buf) {
        return NULL;
    }

    /* trace id */
    sds_cat_safe(&buf, "trace-id: ");

    if (ctx->trace_id) {
        sds_cat_safe(&buf, ctx->trace_id);
    }
    else {
        sds_cat_safe(&buf, "--");
    }


    return buf;
}

void ctr_encode_text_destroy(cfl_sds_t text)
{
    cfl_sds_destroy(text);
}
