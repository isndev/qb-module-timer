
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
