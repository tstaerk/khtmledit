PROJECT( khtmledit )
FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} . )
 

SET(khtmleditSources main.cpp khtmledit.cpp khtmledit.h)

kde4_add_ui_files(khtmleditSources khtmledit.ui)

KDE4_ADD_EXECUTABLE(khtmledit ${khtmleditSources} )

TARGET_LINK_LIBRARIES(khtmledit ${KDE4_KPARTS_LIBS} )

install(TARGETS khtmledit  DESTINATION ${BIN_INSTALL_DIR} )
