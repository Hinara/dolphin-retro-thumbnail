kcoreaddons_add_plugin(ndsthumbnail INSTALL_NAMESPACE "kf6/thumbcreator")

target_sources(ndsthumbnail PRIVATE
    ndsthumbnail.cpp
)

target_link_libraries(ndsthumbnail
    KF6::KIOGui
)
