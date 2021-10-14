
file(GLOB SRC "*.cpp" "*.h")

add_library(midireader ${SRC})

target_link_libraries(midireader)

install(TARGETS midireader DESTINATION lib)