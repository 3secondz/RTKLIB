set(PYTHON_CXX_EXTENSION_SRC
        "_rtklib.cpp"
        "../bindings/convbin/convbin.c"
        "../bindings/rnx2rtkp/rnx2rtkp.c"
        "../bindings/util.cpp"
        )
add_library(_rtklib SHARED ${PYTHON_CXX_EXTENSION_SRC})

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS "-undefined dynamic_lookup")
endif()

message("RTKLIB_PYTHON_C_MODULE_NAME ${RTKLIB_PYTHON_C_MODULE_NAME}")

set_target_properties(
        _rtklib
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${RTKLIB_PYTHON_C_MODULE_NAME}
            LINKER_LANGUAGE CXX
            SUFFIX ".so"
)

if(WIN32)
    set_target_properties(
            _rtklib
            PROPERTIES
            SUFFIX ".pyd"
    )
endif()

target_include_directories(_rtklib PUBLIC
        ${PYTHON_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
        )

target_link_libraries(_rtklib rtklibStatic)

if(WIN32)
    target_link_libraries(_rtklib ${PYTHON_LIBRARIES})
endif()