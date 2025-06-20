paraview_add_plugin(DGtalParaviewPlugin
  REQUIRED_ON_CLIENT
  REQUIRED_ON_SERVER
  VERSION "1.0"
  MODULES 
    IntegralInvariantEstimates
  MODULE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Filters/IntegralInvariantEstimates/vtk.module"
  SERVER_MANAGER_XML DGtal.xml)