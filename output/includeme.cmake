# =============================================
set(library_name "qtimageviewer")
if (WIN32)
    set(${library_name}_include ${CMAKE_CURRENT_SOURCE_DIR}/../${library_name}/output/include)
    include_directories(${${library_name}_include})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(${library_name}_platform "win32msvc")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(${library_name}_platform "win32mingw")
    endif()
    set(${library_name}_lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/../${library_name}/output/lib/${${library_name}_platform}/${library_name})
    target_link_directories(${PROJECT_NAME} PRIVATE ${${library_name}_lib_dir})

    # set(${library_name}_lib1 ${${library_name}_lib_dir}/${library_name}.lib)
    # target_link_libraries(${PROJECT_NAME} PRIVATE ${${library_name}_lib1})

    target_link_libraries(${PROJECT_NAME} PRIVATE ${library_name})
elseif (ANDROID)
    set(${library_name}_include ${CMAKE_CURRENT_SOURCE_DIR}/../${library_name}/output/include)
    include_directories(${${library_name}_include})
    set(${library_name}_platform "android")
    set(${library_name}_lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/../${library_name}/output/lib/${${library_name}_platform}/${library_name})
    set(${library_name}_lib1 ${${library_name}_lib_dir}/lib${library_name}_armeabi-v7a.so)
    target_link_directories(${PROJECT_NAME} PRIVATE ${${library_name}_lib_dir})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${${library_name}_lib1})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${${library_name}_lib1}" "${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}/lib${library_name}_armeabi-v7a.so")
elseif (UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    set(${library_name}_include ${CMAKE_CURRENT_SOURCE_DIR}/../${library_name}/output/include)
    include_directories(${${library_name}_include})
    set(${library_name}_platform "linux64")
    set(${library_name}_lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/../${library_name}/output/lib/${${library_name}_platform}/${library_name})
    set(${library_name}_lib1 ${${library_name}_lib_dir}/lib${library_name}.so)
    target_link_directories(${PROJECT_NAME} PRIVATE ${${library_name}_lib_dir})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${${library_name}_lib1})
endif()
# =============================================


