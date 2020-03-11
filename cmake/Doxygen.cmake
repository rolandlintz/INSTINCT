function(enable_doxygen)
    option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" FALSE)
    if(ENABLE_DOXYGEN)
        find_package(Doxygen
                        REQUIRED dot
                        OPTIONAL_COMPONENTS mscgen dia)

        if(DOXYGEN_FOUND)
            message(STATUS "Doxygen build started")

            # Settings can be looked up here: http://www.doxygen.nl/manual/config.html
            set(DOXYGEN_CALLER_GRAPH YES)
            set(DOXYGEN_CALL_GRAPH YES)
            set(DOXYGEN_EXTRACT_ALL YES)
            set(DOXYGEN_EXCLUDE_PATTERNS "*/lib/vnproglib-1.1.5.0/examples/*")
            set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/doc)
            doxygen_add_docs(doxygen-docs ${PROJECT_SOURCE_DIR} COMMENT "Generate man pages")
        else()
            message(SEND_ERROR "Doxygen need to be installed to generate the doxygen documentation")
        endif()
    endif()
endfunction()
