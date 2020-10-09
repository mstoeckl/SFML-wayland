#
# Try to find Wayland client libraries and include paths.
# Once done this will define
#
# WAYLAND_CLIENT_FOUND
# WAYLAND_CLIENT_INCLUDE_DIRS
# WAYLAND_CLIENT_LIBRARIES
#
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)

mark_as_advanced(WAYLAND_LIBRARIES WAYLAND_CLIENT_LIBRARIES WAYLAND_EGL_LIBRARIES WAYLAND_INCLUDE_DIRS WAYLAND_CLIENT_INCLUDE_DIRS WAYLAND_EGL_INCLUDE_DIRS)

find_program(WAYLAND_SCANNER NAMES wayland-scanner)
