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

#include <qb/actor.h>
#include "../event/Timed.h"
#include "tag.h"

#ifndef QB_MODULE_TIMER_SERVICE_SCHEDULER_H
#define QB_MODULE_TIMER_SERVICE_SCHEDULER_H

namespace qbm {
    namespace timer {
        namespace service {

            class Scheduler
                    : public qb::ServiceActor<Tag>
                    , public qb::Pipe
                    , public qb::ICallback

            {
            public:

                Scheduler() = default;

                bool onInit() override final {
                    registerEvent<event::Timed>(*this);
                    registerEvent<event::Cancel>(*this);
                    this->registerCallback(*this);
                    return true;
                }

                void on(event::Timed const &event) {
                    auto &e = this->recycle(event, event.bucketSize());
                    e.time_id = (reinterpret_cast<CacheLine *>(&e)) - this->data();
                    e.received();
                }

                void on(event::Cancel const &event) {
                    auto &e = *reinterpret_cast<event::Timed *>(this->data() + event.getTimeId());
                    e.live(false);
                    e.release();
                }

                virtual void onCallback() override final {
                    uint64_t const now = qb::NanoTimestamp::nano();

                    if (this->end()) {
                        auto i = this->begin();
                        while (i < this->end()) {
                            auto &event = *reinterpret_cast<event::Timed *>(this->data() + i);
                            bool free_event = false;
                            if (!event.execution_time)
                                free_event = i == this->begin();
                            else if (now >= event.execution_time) {
                                event.release();
                                send(event);
                                free_event = (!event.execution_time && i == this->begin());
                            }
                            if (free_event)
                                this->free_front(event.bucketSize());
                            i += event.bucketSize();
                        }
                        if (this->begin() == this->end())
                            this->reset();
                    }
                }

            };

        }
    }
}

#endif //QB_MODULE_TIMER_SERVICE_SCHEDULER_H
