
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
