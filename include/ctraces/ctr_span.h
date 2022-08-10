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

#ifndef CTR_SPAN_H
#define CTR_SPAN_H

#include <ctraces/ctraces.h>
#include <cfl/cfl_array.h>


/*
 * OpenTelemetry Span Kinds
 * ------------------------
 * https://github.com/open-telemetry/opentelemetry-go/blob/main/trace/trace.go#L423
 */
#define CTRACE_SPAN_UNSPECIFIED   0
#define CTRACE_SPAN_INTERNAL      1
#define CTRACE_SPAN_SERVER        2
#define CTRACE_SPAN_CLIENT        3
#define CTRACE_SPAN_PRODUCER      4
#define CTRACE_SPAN_CONSUMER      5

struct ctrace_span_attribute {
    cfl_sds_t name;
    struct cfl_variant *value;
    struct cfl_list _head;
};

struct ctrace_span_event {
    cfl_sds_t name;
    uint64_t timestamp;

    /* fixme: implement it! */
    struct cfl_kvlist *attributes;

    /* fixme: implement it! */
    void *links;

    struct cfl_list _head;
};

/* span context */
struct ctrace_span {
    uint64_t id;                   /* the unique span ID    */
    uint64_t parent_span_id;       /* any parent ?, id=0 means a root span */

    int kind;                      /* span kind */
    uint64_t start_time;           /* start time */
    uint64_t end_time;             /* end time */

    cfl_sds_t name;                /* user-name assigned */

    struct cfl_kvlist *attributes; /* attributes */
    struct cfl_list events;        /* events */
    struct cfl_list childs;        /* list of child spans */

    /*
     * link to parent list. The root span is linked to 'struct ctrace->spans' and
     * a child span to 'struct ctrace_span->childs'
     */
    struct cfl_list _head;

    struct ctrace *ctx;            /* parent ctrace context */
};

struct ctrace_span *ctr_span_create(struct ctrace *ctx, cfl_sds_t name,
                                    struct ctrace_span *parent);

void ctr_span_destroy(struct ctrace_span *span);

/* attributes */
int ctr_span_set_attribute_string(struct ctrace_span *span, char *key, char *value);
int ctr_span_set_attribute_bool(struct ctrace_span *span, char *key, int b);
int ctr_span_set_attribute_int(struct ctrace_span *span, char *key, int value);
int ctr_span_set_attribute_double(struct ctrace_span *span, char *key, double value);
int ctr_span_set_attribute_array(struct ctrace_span *span, char *key,
                                 struct cfl_array *value);
int ctr_span_set_attribute_kvlist(struct ctrace_span *span, char *key,
                                  struct cfl_kvlist *value);

/* events */
int ctr_span_event_add(struct ctrace_span *span, char *name,
                       void *attributes, void *links);
void ctr_span_event_delete(struct ctrace_span_event *event);

/* time */
void ctr_span_start(struct ctrace *ctx, struct ctrace_span *span);
void ctr_span_end(struct ctrace *ctx, struct ctrace_span *span);

/* kind */
int ctr_span_kind_set(struct ctrace_span *span, int kind);
char *ctr_span_kind_string(struct ctrace_span *span);


#endif
