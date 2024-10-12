# CMake Windows build dependencies module

include_guard(GLOBAL)

include(buildspec_common)

# _check_dependencies_windows: Set up Windows slice for _check_dependencies
function(_check_dependencies_windows)
  set(arch ${CMAKE_GENERATOR_PLATFORM})
  set(platform windows-${arch})

  set(dependencies_dir "${CMAKE_CURRENT_SOURCE_DIR}/.deps")
  set(prebuilt_filename "windows-deps-VERSION-ARCH-REVISION.zip")
  set(prebuilt_destination "obs-deps-VERSION-ARCH")
  set(qt6_filename "windows-deps-qt6-VERSION-ARCH-REVISION.zip")
  set(qt6_destination "obs-deps-qt6-VERSION-ARCH")
  set(obs-studio_filename "VERSION.zip")
  set(obs-studio_destination "obs-studio-VERSION")
  set(dependencies_list prebuilt qt6 obs-studio)

  ##############################################################################
  # OBS-Studio-30.2.3-Windows
  set(obs-studio-testing_filename "OBS-Studio-VERSION-Windows.zip")
  set(obs-studio-testing_destination "obs-studio-VERSION-testing")
  # OBS-Studio-30.2.3-Windows-PDBs
  set(obs-studio-testing-pdbs_filename "OBS-Studio-VERSION-Windows-PDBs.zip")
  set(obs-studio-testing-pdbs_destination "obs-studio-VERSION-testing")
  list(APPEND dependencies_list obs-studio-testing obs-studio-testing-pdbs)
  ##############################################################################

  _check_dependencies()
endfunction()

_check_dependencies_windows()
