
file(GLOB FUNCS_SRC funcs.cpp funcs.h)
add_library(funcs SHARED ${FUNCS_SRC})
target_link_libraries(funcs libmux)
target_include_directories(funcs PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_LIST_DIR})

