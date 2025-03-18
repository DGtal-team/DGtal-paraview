paraview_add_plugin(DGtalParaviewPlugin
  REQUIRED_ON_CLIENT
  REQUIRED_ON_SERVER
  VERSION "1.0"
  MODULES 
    ComputeCurvature
  MODULE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Filters/ComputeCurvature/vtk.module"
  SERVER_MANAGER_XML DGtal.xml)