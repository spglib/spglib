#include <gtest/gtest.h>

#include <list>
#include <thread>

extern "C" {
#include "spglib.h"
void spg_set_error_code(SpglibError err);
}

TEST(Error, thread_safety) {
    auto thread_func = [](SpglibError err_val) {
        spg_set_error_code(err_val);
        // Small delay to make sure other thread has written
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_EQ(spg_get_error_code(), err_val);
    };
    std::list<std::thread> thread_jobs{};
    thread_jobs.emplace_back(thread_func, SPGLIB_SUCCESS);
    thread_jobs.emplace_back(thread_func, SPGERR_NONE);
    for (auto &job : thread_jobs) job.join();
}
