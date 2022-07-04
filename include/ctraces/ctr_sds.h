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

#ifndef CTR_SDS_H
#define CTR_SDS_H

/*
 * This interface is a minimized version of Fluent Bit SDS just for easily
 * string storage
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <ctraces/ctr_log.h>

#define CTR_SDS_HEADER_SIZE (sizeof(uint64_t) + sizeof(uint64_t))

typedef char *ctr_sds_t;

#pragma pack(push, 1)
struct ctr_sds {
    uint64_t len;        /* used */
    uint64_t alloc;      /* excluding the header and null terminator */
    char buf[];
};
#pragma pack(pop)

#define CTR_SDS_HEADER(s)  ((struct ctr_sds *) (s - CTR_SDS_HEADER_SIZE))

static inline void ctr_sds_len_set(ctr_sds_t s, size_t len)
{
    CTR_SDS_HEADER(s)->len = len;
}

size_t ctr_sds_avail(ctr_sds_t s);
ctr_sds_t sds_alloc(size_t size);
size_t ctr_sds_alloc(ctr_sds_t s);
ctr_sds_t ctr_sds_increase(ctr_sds_t s, size_t len);
size_t ctr_sds_len(ctr_sds_t s);
ctr_sds_t ctr_sds_create_len(const char *str, int len);
ctr_sds_t ctr_sds_create(const char *str);
void ctr_sds_destroy(ctr_sds_t s);
ctr_sds_t ctr_sds_cat(ctr_sds_t s, const char *str, int len);
ctr_sds_t ctr_sds_create_size(size_t size);
void ctr_sds_set_len(ctr_sds_t s, size_t len);
void ctr_sds_cat_safe(ctr_sds_t *buf, const char *str, int len);

#endif
