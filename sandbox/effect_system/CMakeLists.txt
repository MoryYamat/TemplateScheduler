add_executable(effect_system ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

set_target_properties(effect_system PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

target_include_directories(effect_system 
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)