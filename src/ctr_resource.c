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
#include <ctraces/ctr_resource.h>

struct ctrace_resource *ctr_resource_create(struct ctrace *ctx)
{
    struct ctrace_resource *res;

    res = calloc(1, sizeof(struct ctrace_resource));
    if (!res) {
        ctr_errno();
        return NULL;
    }
    cfl_list_add(&res->_head, &ctx->resources);

    return res;
}

struct ctrace_resource *ctr_resource_create_default(struct ctrace *ctx)
{
    struct ctrace_resource *res;
    struct ctrace_attributes *attr;

    res = ctr_resource_create(ctx);
    if (!res) {
        return NULL;
    }

    attr = ctr_attributes_create();
    if (!attr) {
        ctr_resource_destroy(res);
        return NULL;
    }

    /* some default attributes */
    ctr_attributes_set_string(attr, "service.name", "Fluent Bit");
    ctr_attributes_set_int64(attr, "release_year", 2014);

    ctr_resource_set_attributes(res, attr);
    return res;
}

int ctr_resource_set_schema_url(struct ctrace_resource *res, char *url)
{
    if (res->schema_url) {
        cfl_sds_destroy(res->schema_url);
    }

    res->schema_url = cfl_sds_create(url);
    if (!res->schema_url) {
        return -1;
    }

    return 0;
}

int ctr_resource_set_attributes(struct ctrace_resource *res, struct ctrace_attributes *attr)
{
    if (!attr) {
        return -1;
    }

    res->attr = attr;

    return 0;
}

void ctr_resource_set_dropped_attr_count(struct ctrace_resource *res, int count)
{
    res->dropped_attr_count = count;
}

void ctr_resource_destroy(struct ctrace_resource *res)
{
    if (res->attr) {
        ctr_attributes_destroy(res->attr);
    }

    if (res->schema_url) {
        cfl_sds_destroy(res->schema_url);
    }

    cfl_list_del(&res->_head);
    free(res);
}
