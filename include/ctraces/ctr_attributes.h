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

#ifndef CTR_ATTRIBUTES_H
#define CTR_ATTRIBUTES_H

#include <ctraces/ctraces.h>

#include <cfl/cfl.h>
#include <cfl/cfl_kvlist.h>

struct ctr_attributes {
    struct cfl_kvlist *kv;
};

struct ctr_attributes *ctr_attributes_create();
void ctr_attributes_destroy(struct ctr_attributes *attr);

int ctr_attributes_set_string(struct ctr_attributes *attr, char *key, char *value);
int ctr_attributes_set_bool(struct ctr_attributes *attr, char *key, int b);
int ctr_attributes_set_int(struct ctr_attributes *attr, char *key, int value);
int ctr_attributes_set_double(struct ctr_attributes *attr, char *key, double value);
int ctr_attributes_set_array(struct ctr_attributes *attr, char *key,
                             struct cfl_array *value);
int ctr_attributes_set_kvlist(struct ctr_attributes *attr, char *key,
                              struct cfl_kvlist *value);

#endif
