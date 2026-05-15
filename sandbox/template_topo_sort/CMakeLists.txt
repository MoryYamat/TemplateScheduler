add_executable(sb_topo ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

set_target_properties(sb_topo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

target_include_directories(sb_topo 
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)