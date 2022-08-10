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

#ifndef CTR_H
#define CTR_H

#define CTR_FALSE   0
#define CTR_TRUE    !CTR_FALSE

/* local libs */
#include <cfl/cfl.h>

/* headers that are needed in general */
#include <ctraces/ctr_info.h>
#include <ctraces/ctr_version.h>
#include <ctraces/ctr_span.h>
#include <ctraces/ctr_log.h>


#include <stdio.h>
#include <stdlib.h>

/* ctrace options creation keys */
#define CTR_OPTS_TRACE_ID   0

struct ctrace_opts {
    cfl_sds_t  trace_id;
};

struct ctrace {
    /* trace context */
    cfl_sds_t trace_id;

    /*
     * last_span_id represents the higher span id number assigned, every time
     * a new span is created this value gets incremented.
     */
    uint64_t last_span_id;
    struct cfl_list spans;

    /* logging */
    int log_level;
    void (*log_cb)(void *, int, const char *, int, const char *);
};

struct ctrace *ctr_create(struct ctrace_opts *opts);
void ctr_destroy(struct ctrace *ctx);

/* options */
void ctr_opts_init(struct ctrace_opts *opts);
void ctr_opts_set(struct ctrace_opts *opts, int value, char *val);
void ctr_opts_exit(struct ctrace_opts *opts);

#endif
