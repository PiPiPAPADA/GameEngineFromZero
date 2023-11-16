add_executable(GeomMathTest GeomMathTest.cpp)
target_link_libraries(GeomMathTest GeomMath)
add_test(NAME Test_GeomMath COMMAND GeomMathTest)

add_executable(SceneObjectTest SceneObjectTest.cpp)
target_link_libraries(SceneObjectTest Common ${XG_LIBRARY})
__add_xg_platform_dependencies(SceneObjectTest)
add_test(NAME TEST_SceneObject COMMAND SceneObjectTest)

add_executable(ColorSpaceConversionTest ColorSpaceConversionTest.cpp)
target_link_libraries(ColorSpaceConversionTest GeomMath)
add_test(NAME Test_ColorSpaceConversion COMMAND ColorSpaceConversionTest)

add_executable(OgexParserTest OgexParserTest.cpp)
target_link_libraries(OgexParserTest Common ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY})
__add_xg_platform_dependencies(OgexParserTest)
add_test(NAME TEST_OgexParser COMMAND OgexParserTest)

add_executable(JpegParserTest JpegParserTest.cpp)
target_link_libraries(JpegParserTest Common)
add_test(NAME TEST_JpegParser COMMAND JpegParserTest)