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

/*
 * This interface is a minimized version of Fluent Bit SDS just for easily
 * string storage
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <ctraces/ctr_sds.h>
#include <ctraces/ctr_log.h>

size_t ctr_sds_avail(ctr_sds_t s)
{
    struct ctr_sds *h;

    h = CTR_SDS_HEADER(s);
    return (size_t) (h->alloc - h->len);
}

ctr_sds_t sds_alloc(size_t size)
{
    void *buf;
    ctr_sds_t s;
    struct ctr_sds *head;

    buf = malloc(CTR_SDS_HEADER_SIZE + size + 1);
    if (!buf) {
        ctr_errno();
        return NULL;
    }

    head = buf;
    head->len = 0;
    head->alloc = size;

    s = head->buf;
    *s = '\0';

    return s;
}

size_t ctr_sds_alloc(ctr_sds_t s)
{
    return (size_t) CTR_SDS_HEADER(s)->alloc;
}

ctr_sds_t ctr_sds_increase(ctr_sds_t s, size_t len)
{
    size_t new_size;
    struct ctr_sds *head;
    ctr_sds_t out;
    void *tmp;

    out = s;
    new_size = (CTR_SDS_HEADER_SIZE + ctr_sds_alloc(s) + len + 1);
    head = CTR_SDS_HEADER(s);
    tmp = realloc(head, new_size);
    if (!tmp) {
        ctr_errno();
        return NULL;
    }
    head = (struct ctr_sds *) tmp;
    head->alloc += len;
    out = head->buf;

    return out;
}

size_t ctr_sds_len(ctr_sds_t s)
{
    return (size_t) CTR_SDS_HEADER(s)->len;
}

ctr_sds_t ctr_sds_create_len(const char *str, int len)
{
    ctr_sds_t s;
    struct ctr_sds *head;

    s = sds_alloc(len);
    if (!s) {
        return NULL;
    }

    if (str) {
        memcpy(s, str, len);
        s[len] = '\0';

        head = CTR_SDS_HEADER(s);
        head->len = len;
    }
    return s;
}

ctr_sds_t ctr_sds_create(const char *str)
{
    size_t len;

    if (!str) {
        len = 0;
    }
    else {
        len = strlen(str);
    }

    return ctr_sds_create_len(str, len);
}

void ctr_sds_destroy(ctr_sds_t s)
{
    struct ctr_sds *head;

    if (!s) {
        return;
    }

    head = CTR_SDS_HEADER(s);
    free(head);
}

ctr_sds_t ctr_sds_cat(ctr_sds_t s, const char *str, int len)
{
    size_t avail;
    struct ctr_sds *head;
    ctr_sds_t tmp = NULL;

    avail = ctr_sds_avail(s);
    if (avail < len) {
        tmp = ctr_sds_increase(s, len);
        if (!tmp) {
            return NULL;
        }
        s = tmp;
    }
    memcpy((char *) (s + ctr_sds_len(s)), str, len);

    head = CTR_SDS_HEADER(s);
    head->len += len;
    s[head->len] = '\0';

    return s;
}

ctr_sds_t ctr_sds_create_size(size_t size)
{
    return sds_alloc(size);
}

void ctr_sds_set_len(ctr_sds_t s, size_t len)
{
    struct ctr_sds *head;

    head = CTR_SDS_HEADER(s);
    head->len = len;
}

void ctr_sds_cat_safe(ctr_sds_t *buf, const char *str, int len)
{
    ctr_sds_t tmp;

    tmp = ctr_sds_cat(*buf, str, len);
    if (!tmp) {
        return;
    }
    *buf = tmp;
}

