
file(GLOB SRC "*.cpp" "*.h" "*.hpp")

add_library(oscreader ${SRC})

target_link_libraries(oscreader)

install(TARGETS oscreader DESTINATION lib)