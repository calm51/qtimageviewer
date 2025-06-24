cmake_minimum_required(VERSION 3.16)

project(qtimageviewer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

add_library(qtimageviewer SHARED
  src/qtgllogger_obj.h
  src/qtgllogger_obj.cpp
  src/qtimageviewer_global.h
  src/qtimageviewer.cpp
  src/qtimageviewer.h
  src/qtimageviewer.ui
  src/qtimageviewer.h src/qtimageviewer.cpp src/qtimageviewer.ui

  src/scene/checkerboardscene.h src/scene/checkerboardscene.cpp
  src/scene/canvasrectitem.h src/scene/canvasrectitem.cpp
  src/scene/handlerectitem.h src/scene/handlerectitem.cpp

  src/elements/clippedpixmapitem.h src/elements/clippedpixmapitem.cpp
  src/elements/clippedtextitem.h src/elements/clippedtextitem.cpp
  src/elements/clippedrectitem.h src/elements/clippedrectitem.cpp

  src/editboard/editboard.h src/editboard/editboard.cpp src/editboard/editboard.ui
  src/editboard/editboard_addelement.h src/editboard/editboard_addelement.cpp src/editboard/editboard_addelement.ui
  src/editboard/editboard_adjustelement_text.h src/editboard/editboard_adjustelement_text.cpp src/editboard/editboard_adjustelement_text.ui
  src/editboard/editboard_adjustelement_rect.h src/editboard/editboard_adjustelement_rect.cpp src/editboard/editboard_adjustelement_rect.ui

  src/utils.h src/utils.cpp
  src/scene/graphicsview.h src/scene/graphicsview.cpp
)

target_link_libraries(qtimageviewer PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(qtimageviewer PRIVATE QTIMAGEVIEWER_LIBRARY)

if (WIN32)
elseif (ANDROID)
  find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS AndroidExtras)
  find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS AndroidExtras)
  target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::AndroidExtras)
elseif (UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
endif()

include("./output/makeme.cmake")

include("../qtgllogger/output/includeme.cmake")

include("../qtframeless/output/includeme.cmake")


