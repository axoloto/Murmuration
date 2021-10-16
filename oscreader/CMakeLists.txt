
file(GLOB SRC "*.cpp" "*.h")

add_library(oscreader ${SRC})

target_link_libraries(oscreader)

install(TARGETS oscreader DESTINATION lib)