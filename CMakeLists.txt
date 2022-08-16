cmake_minimum_required(VERSION 3.16)
project(ndsthumbnail LANGUAGES CXX)

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.90.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMOptionalAddSubdirectory)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Gui)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS KIO)
add_definitions(-DQT_USE_QSTRINGBUILDER)

ecm_optional_add_subdirectory(nds)
ecm_optional_add_subdirectory(gcn)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
