

if (QB_BUILD_TEST)
    qb_register_module_gtest(
            NAME ${PROJECT_NAME}
            TESTNAME scheduler
            SOURCES test-scheduler.cpp
    )
endif()