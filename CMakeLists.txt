# A name for the project
project(plasma-chatelleraultengine)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

# We add our source code here
set(chatellerault_engine_SRCS chatellerault.cpp)

# Now make sure all files get to the right place
kde4_add_plugin(plasma_engine_chatellerault ${chatellerault_engine_SRCS})
target_link_libraries(plasma_engine_chatellerault
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_PLASMA_LIBS})

install(TARGETS plasma_engine_chatellerault
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-engine-chatellerault.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

