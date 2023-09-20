
        SET(GEOMMATH_LIB_FILE ${CMAKE_CURRENT_BINARY_DIR}/GeomMath.lib)
        SET(ISPC_COMPILER ispc.exe)
        SET(ISPC_COMPILER_PATH ${PROJECT_SOURCE_DIR}/External/ispc/bin/)
        SET(ISPC_OPTIONS --arch=x86-64 --target=host)
        SET(LIBRARIAN_COMMAND lib)
        SET(LIBRARIAN_COMMAND lib /MACHINE:X64 /OUT:${GEOMMATH_LIB_FILE})


SET(GEOMMATH_LIB_HEADER_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(GeomMath STATIC IMPORTED GLOBAL)
add_dependencies(GeomMath GeomMathLib)

set_target_properties(GeomMath
        PROPERTIES
        IMPORTED_LOCATION ${GEOMMATH_LIB_FILE}
        INTERFACE_INCLUDE_DIRECTORIES ${GEOMMATH_LIB_HEADER_FOLDER}
        )

add_subdirectory(ispc)
# add_subdirectory(test)
