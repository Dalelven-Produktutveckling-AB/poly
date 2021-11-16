find_package(Threads)

add_executable(timer timer.cpp)
target_link_libraries(timer poly::poly Threads::Threads)

add_executable(irq_with_data irq_with_data.cpp)
target_link_libraries(irq_with_data poly::poly Threads::Threads)

add_executable(irq_set_with_data irq_set_with_data.cpp)
target_link_libraries(irq_set_with_data poly::poly Threads::Threads)
