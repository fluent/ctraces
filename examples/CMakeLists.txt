set(src
	# simple.c
	otlp.c
	)

# add_executable(ctraces-simple ${src})
# target_link_libraries(ctraces-simple ctraces-static)
add_executable(ctraces-otlp ${src})
target_link_libraries(ctraces-otlp ctraces-static)

configure_file(sample_trace.bin sample_trace.bin COPYONLY)