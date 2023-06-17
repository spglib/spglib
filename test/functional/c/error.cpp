#include <gtest/gtest.h>

#include <list>
#include <random>
#include <thread>

extern "C" {
#include "spglib.h"
}

TEST(Error, test_spg_get_error_message) {
    // printf("Primitive cell was not found.\n");
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1, 1};
    int num_atom = 3;
    int num_primitive_atom;
    double symprec = 1e-5;
    SpglibError error;

    /* lattice, position, and types are overwritten. */
    num_primitive_atom =
        spg_find_primitive(lattice, position, types, num_atom, symprec);
    ASSERT_EQ(num_primitive_atom, 0);

    error = spg_get_error_code();
    printf("%s\n", spg_get_error_message(error));
    ASSERT_NE(error, SPGLIB_SUCCESS);
}

extern "C" {
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
    for (auto& job : thread_jobs) job.join();
}
