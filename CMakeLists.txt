cmake_minimum_required(VERSION 3.10)
project(trimesh2 VERSION 2.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    add_compile_options(-Wno-deprecated-declarations)
endif()

# Options
option(BUILD_GLUIT "Build gluit library" ON)
option(BUILD_UTILS "Build utility programs" ON)
option(BUILD_TESTING "Build unit tests" ON)
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

# Include directories
include_directories(include)

if(ENABLE_COVERAGE)
    add_compile_options(--coverage)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()

# Platform specific settings
if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
    set(OPENGL_LIBS "-framework OpenGL" "-framework GLUT")
    set(DONT_BUILD_FREEGLUT ON)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
    set(OPENGL_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif()

# trimesh library
set(TRIMESH_SOURCES
    libsrc/TriMesh_bounding.cc
    libsrc/TriMesh_connectivity.cc
    libsrc/TriMesh_curvature.cc
    libsrc/TriMesh_io.cc
    libsrc/TriMesh_grid.cc
    libsrc/TriMesh_normals.cc
    libsrc/TriMesh_pointareas.cc
    libsrc/TriMesh_stats.cc
    libsrc/TriMesh_tstrips.cc
    libsrc/GLCamera.cc
    libsrc/GLManager.cc
    libsrc/ICP.cc
    libsrc/KDtree.cc
    libsrc/conn_comps.cc
    libsrc/diffuse.cc
    libsrc/edgeflip.cc
    libsrc/faceflip.cc
    libsrc/filter.cc
    libsrc/make.cc
    libsrc/merge.cc
    libsrc/overlap.cc
    libsrc/remove.cc
    libsrc/reorder_verts.cc
    libsrc/subdiv.cc
    libsrc/umbrella.cc
)

add_library(trimesh STATIC ${TRIMESH_SOURCES})

# gluit library
if(BUILD_GLUIT)
    set(GLUIT_SOURCES
        gluit/glui.cc
        gluit/glui_add_controls.cc
        gluit/glui_algebra3.cc
        gluit/glui_arcball.cc
        gluit/glui_bitmap_img_data.cc
        gluit/glui_bitmaps.cc
        gluit/glui_button.cc
        gluit/glui_checkbox.cc
        gluit/glui_column.cc
        gluit/glui_control.cc
        gluit/glui_edittext.cc
        gluit/glui_listbox.cc
        gluit/glui_mouse_iaction.cc
        gluit/glui_node.cc
        gluit/glui_panel.cc
        gluit/glui_quaternion.cc
        gluit/glui_radio.cc
        gluit/glui_rollout.cc
        gluit/glui_rotation.cc
        gluit/glui_separator.cc
        gluit/glui_slider.cc
        gluit/glui_spinner.cc
        gluit/glui_statictext.cc
        gluit/glui_translation.cc
    )
    
    if(NOT DONT_BUILD_FREEGLUT)
        list(APPEND GLUIT_SOURCES
            gluit/freeglut_callbacks.c
            gluit/freeglut_cursor.c
            gluit/freeglut_display.c
            gluit/freeglut_ext.c
            gluit/freeglut_font.c
            gluit/freeglut_font_data.c
            gluit/freeglut_gamemode.c
            gluit/freeglut_geometry.c
            gluit/freeglut_glutfont_definitions.c
            gluit/freeglut_init.c
            gluit/freeglut_input_devices.c
            gluit/freeglut_joystick.c
            gluit/freeglut_main.c
            gluit/freeglut_menu.c
            gluit/freeglut_misc.c
            gluit/freeglut_overlay.c
            gluit/freeglut_spaceball.c
            gluit/freeglut_state.c
            gluit/freeglut_stroke_mono_roman.c
            gluit/freeglut_stroke_roman.c
            gluit/freeglut_structure.c
            gluit/freeglut_teapot.c
            gluit/freeglut_videoresize.c
            gluit/freeglut_window.c
            gluit/freeglut_xinput.c
        )
    endif()

    add_library(gluit STATIC ${GLUIT_SOURCES})
    target_include_directories(gluit PRIVATE include/GL)
endif()

# Utilities
if(BUILD_UTILS)
    set(UTILS
        mesh_align
        mesh_cat
        mesh_cc
        mesh_check
        mesh_crunch
        mesh_filter
        mesh_hf
        mesh_info
        mesh_make
        mesh_shade
        grid_subsamp
        xf
    )

    foreach(util ${UTILS})
        add_executable(${util} utilsrc/${util}.cc)
        target_link_libraries(${util} trimesh)
        install(TARGETS ${util} DESTINATION ${CMAKE_INSTALL_BINDIR})
    endforeach()

    if(BUILD_GLUIT)
        add_executable(mesh_view utilsrc/mesh_view.cc)
        target_link_libraries(mesh_view trimesh gluit ${OPENGL_LIBS})
        install(TARGETS mesh_view DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()

# Installation
install(TARGETS trimesh DESTINATION ${CMAKE_INSTALL_LIBDIR})
if(BUILD_GLUIT)
    install(TARGETS gluit DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Tests
if(BUILD_TESTING)
    enable_testing()
    add_executable(all_tests tests/all_tests.cc)
    target_link_libraries(all_tests trimesh)
    add_test(NAME all_tests COMMAND all_tests)
endif()
