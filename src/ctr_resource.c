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

struct ctrace_resource *ctr_resource_create()
{
    struct ctrace_resource *res;

    res = calloc(1, sizeof(struct ctrace_resource));
    if (!res) {
        ctr_errno();
        return NULL;
    }

    return res;
}

struct ctrace_resource *ctr_resource_create_default()
{
    struct ctrace_resource *res;
    struct ctrace_attributes *attr;

    res = ctr_resource_create();
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

int ctr_resource_set_attributes(struct ctrace_resource *res, struct ctrace_attributes *attr)
{
    if (!attr) {
        return -1;
    }

    res->attr = attr;
    return 0;
}

void ctr_resource_set_dropped_attr_count(struct ctrace_resource *res, uint32_t count)
{
    res->dropped_attr_count = count;
}

void ctr_resource_destroy(struct ctrace_resource *res)
{
    if (res->attr) {
        ctr_attributes_destroy(res->attr);
    }
    free(res);
}

/*
 * resource_span API
 * -----------------
 */

/* creates a resource_span context */
struct ctrace_resource_span *ctr_resource_span_create(struct ctrace *ctx)
{
    struct ctrace_resource_span *resource_span;

    resource_span = calloc(1, sizeof(struct ctrace_resource_span));
    if (!resource_span) {
        ctr_errno();
        return NULL;
    }
    cfl_list_init(&resource_span->scope_spans);

    /* link to ctraces context */
    cfl_list_add(&resource_span->_head, &ctx->resource_spans);

    return resource_span;
}

/* Set the schema_url for a resource_span */
int ctr_resource_span_set_schema_url(struct ctrace_resource_span *resource_span, char *url)
{
    if (resource_span->schema_url) {
        cfl_sds_destroy(resource_span->schema_url);
    }

    resource_span->schema_url = cfl_sds_create(url);
    if (!resource_span->schema_url) {
        return -1;
    }

    return 0;
}

void ctr_resource_span_set_resource(struct ctrace_resource_span *resource_span,
                                    struct ctrace_resource *resource)
{
    resource_span->resource = resource;
}

void ctr_resource_span_destroy(struct ctrace_resource_span *resource_span)
{
    struct cfl_list *tmp;
    struct cfl_list *head;
    struct ctrace_scope_span *scope_span;

    /* release resource if set */
    if (resource_span->resource) {
        ctr_resource_destroy(resource_span->resource);
    }

    if (resource_span->schema_url) {
        cfl_sds_destroy(resource_span->schema_url);
    }

    /* remove scope spans */
    cfl_list_foreach_safe(head, tmp, &resource_span->scope_spans) {
        scope_span = cfl_list_entry(head, struct ctrace_scope_span, _head);
        ctr_scope_span_destroy(scope_span);
    }

    free(resource_span);
}
