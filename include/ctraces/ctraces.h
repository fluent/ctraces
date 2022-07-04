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

/* headers that are needed in general */
#include <ctraces/ctr_info.h>
#include <ctraces/ctr_version.h>
#include <ctraces/ctr_log.h>

#include <stdio.h>
#include <stdlib.h>

struct ctrace {
    /* logging */
    int log_level;
    void (*log_cb)(void *, int, const char *, int, const char *);
};

struct ctrace *ctr_create();
void ctr_destroy(struct ctrace *ctx);

#endif
