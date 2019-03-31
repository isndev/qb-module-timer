/*
 * qb - C++ Actor Framework
 * Copyright (C) 2011-2019 isndev (www.qbaf.io). All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *         limitations under the License.
 */

#include <qb/event.h>

#ifndef QB_MODULE_TIMER_EVENT_CANCEL_H
#define QB_MODULE_TIMER_EVENT_CANCEL_H

namespace qbm {
    namespace timer {
        namespace event {
            // input event
            class Cancel
                    : public qb::Event {
                uint64_t time_id;
            public:
                Cancel() = delete;
                Cancel(uint64_t const id)
                        : time_id(id) {}

                uint64_t getTimeId() const {
                    return time_id;
                }

            };
        }
    }
}

#endif //QB_MODULE_TIMER_EVENT_CANCEL_H
