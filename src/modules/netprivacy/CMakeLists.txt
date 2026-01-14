cmake_minimum_required(VERSION 3.16)
project(calamares-netprivacy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Quick QuickWidgets)

find_path(CALAMARES_INCLUDE_DIR
    NAMES libcalamares/CppJob.h
    PATHS /usr/include /usr/include/calamares
)

find_library(CALAMARES_LIBRARY
    NAMES calamares
    PATHS /usr/lib /usr/lib64 /usr/lib/x86_64-linux-gnu
)

add_library(calamares_viewmodule_netprivacy SHARED
    NetPrivacyJob.cpp
    NetPrivacyViewStep.cpp
    netprivacy.qrc
)

target_include_directories(calamares_viewmodule_netprivacy PRIVATE
    ${CALAMARES_INCLUDE_DIR}
    ${CALAMARES_INCLUDE_DIR}/libcalamares
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(calamares_viewmodule_netprivacy PRIVATE
    ${CALAMARES_LIBRARY}
    Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Quick Qt6::QuickWidgets
)

target_compile_definitions(calamares_viewmodule_netprivacy PRIVATE
    PLUGINDLLEXPORT=Q_DECL_EXPORT
)

install(TARGETS calamares_viewmodule_netprivacy
    LIBRARY DESTINATION lib/calamares/modules/netprivacy
)

install(FILES module.desc netprivacy.conf
    DESTINATION lib/calamares/modules/netprivacy
)
