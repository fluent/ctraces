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

#ifndef CTR_ID_H
#define CTR_ID_H

#define CTR_ID_BUFFER_SIZE    16

struct ctrace_id {
    uint8_t buf[CTR_ID_BUFFER_SIZE];
};

int ctr_id_init(struct ctrace_id *cid);
void ctr_id_set(struct ctrace_id *cid, void *buf);
int ctr_id_cmp(struct ctrace_id *cid1, struct ctrace_id *cid2);
cfl_sds_t ctr_id_to_lower_base16(struct ctrace_id *cid);

#endif
