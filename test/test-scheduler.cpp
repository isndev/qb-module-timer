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
    const qb::ActorId _to;
public:
    explicit TestActor(qb::CoreId const core = 0)
            : _to(getServiceId<qbm::timer::service::Tag>(core))
    {}

    virtual bool onInit() override final {
        registerEvent<TestEvent>(*this);

        auto &e = push<TestEvent>(_to, qb::Timespan::seconds(1));
        e.repeat = 3;
        e.forward = id();

        return true;
    }

    void on(TestEvent &event) {
        qb::io::cout() << "event" << event.repeat << std::endl;
        if (!event.repeat) {
            push<qb::KillEvent>(_to);
            kill();
        } else
            event.live(true);
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