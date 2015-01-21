#
# if we have doxygen, make dem docs
#
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(doxyfile_in ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
    set(doxyfile ${PROJECT_BINARY_DIR}/docs/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(gtek_docs ALL ${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/docs
        COMMENT "Generating HTML documentation"
        VERBATIM)
endif()
