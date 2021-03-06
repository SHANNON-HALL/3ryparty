cmake_minimum_required(VERSION 3.6)
project(jbigkit)


set(JBIG_SRC libjbig/jbig.c libjbig/jbig_ar.c)
set(JBIG85_SRC libjbig/jbig85.c libjbig/jbig_ar.c libjbig/jbig_ar.h)

add_library(libjbig STATIC ${JBIG_SRC})
set_target_properties(libjbig PROPERTIES PUBLIC_HEADER "libjbig/jbig_ar.h;libjbig/jbig.h")
add_library(libjbig85 STATIC ${JBIG85_SRC})
set_target_properties(libjbig85 PROPERTIES PUBLIC_HEADER "libjbig/jbig_ar.h;libjbig/jbig85.h")

add_executable(pbmtojbg pbmtools/pbmtojbg.c)
add_executable(pbmtojbg85 pbmtools/pbmtojbg85.c)
add_executable(jbgtopbm pbmtools/pbmtojbg.c)
add_executable(jbgtopbm85 pbmtools/pbmtojbg85.c)

target_include_directories(pbmtojbg PRIVATE libjbig)
target_include_directories(pbmtojbg85 PRIVATE libjbig)
target_include_directories(jbgtopbm PRIVATE libjbig)
target_include_directories(jbgtopbm85 PRIVATE libjbig)

target_link_libraries(pbmtojbg libjbig)
target_link_libraries(pbmtojbg85 libjbig85)
target_link_libraries(jbgtopbm libjbig)
target_link_libraries(jbgtopbm85 libjbig85)

install(TARGETS libjbig libjbig85 pbmtojbg pbmtojbg85 jbgtopbm jbgtopbm85
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )