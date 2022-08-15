
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

########### next target ###############

set(ndsthumbnail_SRCS
	ndsthumbnail.cpp
)

find_package(KF5 REQUIRED
    KIO
)

add_library(ndsthumbnail MODULE ${ndsthumbnail_SRCS})
target_link_libraries(ndsthumbnail KF5::KIOCore KF5::KIOWidgets Qt::Gui)
install(TARGETS ndsthumbnail DESTINATION ${KDE_INSTALL_PLUGINDIR})


########### install files ###############

install( FILES ndsthumbnail.desktop  DESTINATION  ${KDE_INSTALL_KSERVICESDIR} )

