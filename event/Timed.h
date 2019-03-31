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

#include "Cancel.h"

#ifndef QB_MODULE_TIMER_EVENT_TIMED_H
#define QB_MODULE_TIMER_EVENT_TIMED_H

namespace qbm {
    namespace timer {
        namespace event {
            // input/output event
            struct Timed
                    : public qb::ServiceEvent {
                friend class Scheduler;
                uint64_t time_id;
                uint64_t start_time;
                uint64_t execution_time;
            public:
                uint32_t repeat;

                Timed() = delete;

                Timed(qb::Timespan const &span, uint32_t const repeat = 1)
                        : start_time(qb::Timestamp::nano())
                        , execution_time(start_time + span.nanoseconds())
                        , repeat(repeat)
                {
                    service_event_id = qb::type_id<Timed>();
                    live(true);
                }

                template<typename _Event, typename _Actor>
                inline void cancel(_Actor &actor) {
                    actor.template unregisterEvent<_Event>();
                    actor.template send<Cancel>(forward, time_id);
                    live(false);
                }

                void release() {
                    if (isLive() && qb::likely(static_cast<bool>(--repeat))) {
                        auto now = execution_time;
                        execution_time = now + (now - start_time);
                        start_time = now;
                    } else {
                        execution_time = 0;
                        live(false);
                    }
                }
            };

        }
    }
}

#endif //QB_MODULE_TIMER_EVENT_TIMED_H
