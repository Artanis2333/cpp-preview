include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(co_await co_await.cpp)
add_executable(co_await_when_all co_await_when_all.cpp)
add_executable(co_exception co_exception.cpp)
add_executable(co_yield_generator co_yield_generator.cpp)
add_executable(switch_thread switch_thread.cpp)

target_link_libraries(co_await pthread)
target_link_libraries(co_await_when_all pthread)
target_link_libraries(switch_thread pthread)
