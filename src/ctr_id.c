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

int ctr_id_init(struct ctrace_id *cid)
{
    ssize_t ret;

    ret = ctr_random_get(&cid->buf, CTR_ID_BUFFER_SIZE);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

void ctr_id_set(struct ctrace_id *cid, void *buf)
{
    memcpy(&cid->buf, buf, CTR_ID_BUFFER_SIZE);
}

int ctr_id_cmp(struct ctrace_id *cid1, struct ctrace_id *cid2)
{
    if (memcmp(&cid1->buf, &cid2->buf, CTR_ID_BUFFER_SIZE) == 0) {
        return 0;
    }

    return -1;
}

cfl_sds_t ctr_id_to_lower_base16(struct ctrace_id *cid)
{
    int i;
    cfl_sds_t out;
    const char hex[] = "0123456789abcdef";

    out = cfl_sds_create_size(CTR_ID_BUFFER_SIZE * 2);
    if (!out) {
        return NULL;
    }

    for (i = 0; i < CTR_ID_BUFFER_SIZE; i++) {
        out[i * 2] = hex[(cid->buf[i] >> 4) & 0xF];
        out[i * 2 + 1] = hex[(cid->buf[i] >> 0) & 0xF];
    }

    return out;
}
