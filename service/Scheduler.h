
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
                    uint64_t const now = this->time();

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
