add_executable(borsh_test borsh.cpp)
target_include_directories(borsh_test PRIVATE ${CMAKE_SOURCE_DIR}/third-party)
target_link_libraries(borsh_test PRIVATE borsh)

