set(gcnthumbnail_SRCS
	gcnthumbnail.cpp
)

find_package(KF5 REQUIRED
    KIO
)

add_library(gcnthumbnail MODULE ${gcnthumbnail_SRCS})
target_include_directories(gcnthumbnail PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gcnthumbnail KF5::KIOCore KF5::KIOWidgets Qt::Gui)
install(TARGETS gcnthumbnail DESTINATION ${KDE_INSTALL_PLUGINDIR})


########### install files ###############

install( FILES gcnthumbnail.desktop  DESTINATION  ${KDE_INSTALL_KSERVICESDIR} )

