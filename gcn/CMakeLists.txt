kcoreaddons_add_plugin(gcnthumbnail INSTALL_NAMESPACE "kf6/thumbcreator")

target_sources(gcnthumbnail PRIVATE
    gcnthumbnail.cpp
)

target_link_libraries(gcnthumbnail
    KF6::KIOGui
)
