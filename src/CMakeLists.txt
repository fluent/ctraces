set(src
  ctraces.c
  ctr_log.c
  ctr_utils.c
  )

# Static Library
add_library(ctraces-static STATIC ${src})
target_link_libraries(ctraces-static mpack-static)

# Install Library
if(MSVC)
  # Rename the output for Windows environment to avoid naming issues
  set_target_properties(ctraces-static PROPERTIES OUTPUT_NAME libctraces)
else()
  set_target_properties(ctraces-static PROPERTIES OUTPUT_NAME ctraces)
endif(MSVC)

install(TARGETS ctraces-static
  RUNTIME DESTINATION ${CTR_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CTR_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CTR_INSTALL_LIBDIR}
  COMPONENT library)
