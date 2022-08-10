set(src
	simple.c
	)

add_executable(ctraces-simple ${src})
target_link_libraries(ctraces-simple ctraces-static)

