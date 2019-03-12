#include <gtest/gtest.h>
#include <qb/main.h>
#include "../service/Scheduler.h"

struct TestEvent : qbm::timer::event::Timed
{
    std::vector<int> _dynamic;

    TestEvent(qb::Timespan const &span, uint32_t const repeat = 1)
        : Timed(span, repeat)
    {
        _dynamic.resize(4096);
    }
};

class TestActor : public qb::Actor {
    qb::ActorId _to;
public:
    TestActor(uint16_t const core = 0)
            : _to(getServiceId<qbm::timer::service::Tag>(core))
    {}

    virtual bool onInit() override final {
        registerEvent<TestEvent>(*this);

        auto &e = push<TestEvent>(_to, qb::Timespan::seconds(1));
        e.repeat = 3;
        e.forward = id();


        return true;
    }

    void on(TestEvent const &event) {
        qb::io::cout() << "event" << event.repeat << std::endl;
        if (!event.repeat) {
            push<qb::KillEvent>(_to);
            kill();
        }
    }
};

TEST(ModuleTimer, MonoTimedEvent) {
    qb::Main main({0});

    main.core(0)
            .addActor<qbm::timer::service::Scheduler>()
            .addActor<TestActor>();

    main.start();
    main.join();
    EXPECT_FALSE(main.hasError());
}

TEST(ModuleTimer, MultiTimedEvent) {
    qb::Main main(qb::CoreSet::build());
    const auto max_core = std::thread::hardware_concurrency();

    for (auto i = 0; i < max_core; ++i) {
        main.core(i)
                .addActor<qbm::timer::service::Scheduler>()
                .addActor<TestActor>(i);
    }

    main.start();
    main.join();
    EXPECT_FALSE(main.hasError());
}