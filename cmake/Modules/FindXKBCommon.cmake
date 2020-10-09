#
# Try to find Wayland client libraries and include paths.
# Once done this will define
#
# WAYLAND_CLIENT_FOUND
# WAYLAND_CLIENT_INCLUDE_DIRS
# WAYLAND_CLIENT_LIBRARIES
#
find_package(PkgConfig REQUIRED)
pkg_check_modules(XKB_COMMON REQUIRED xkbcommon)

mark_as_advanced(XKB_COMMON_LIBRARIES XKB_COMMON_INCLUDE_DIRS)

find_program(WAYLAND_SCANNER NAMES wayland-scanner)
