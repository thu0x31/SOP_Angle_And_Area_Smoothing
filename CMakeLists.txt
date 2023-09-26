cmake_minimum_required( VERSION 3.24 )

project( HDK_SOP_Angle_And_Area_Smoothing )

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        set(VCPKG_BUILD_TYPE release)
    endif (MSVC)
endif()

find_package( Houdini REQUIRED )
find_package( CGAL REQUIRED )
find_package( Ceres REQUIRED )

set( library_name SOP_Angle_And_Area_Smoothing )

houdini_generate_proto_headers( FILES  src/SOP_Angle_And_Area_Smoothing.cpp )

add_library( ${library_name} SHARED
    src/SOP_Angle_And_Area_Smoothing.cpp
    src/SOP_Angle_And_Area_Smoothing.hpp
)

target_compile_definitions( ${library_name} PRIVATE CGAL_PMP_USE_CERES_SOLVER )
target_link_libraries( ${library_name} Houdini CGAL Ceres::ceres)

target_include_directories( ${library_name} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

houdini_configure_target( ${library_name} )

if (MSVC)
    set(DLL_NAMES gmp-10.dll glog.dll ceres.dll gflags.dll)

    foreach(DLL_NAME ${DLL_NAMES})
        set(DLL_TARGET $<TARGET_FILE_DIR:${library_name}>/${DLL_NAME})
        set(DLL_OUTPUT_DIR $<TARGET_FILE_DIR:${library_name}>/../bin)
        set(DLL_OUTPUT ${DLL_OUTPUT_DIR}/${DLL_NAME})

        add_custom_command(
            TARGET ${library_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL_TARGET}
                ${DLL_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E remove
                ${DLL_TARGET}
            COMMENT "moved ${DLL_NAME}"
        )
    endforeach()
endif (MSVC)

# TODO: icon