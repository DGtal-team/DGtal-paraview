set(classes
  vtkComputeCurvature)

vtk_module_add_module(ComputeCurvature
  CLASSES ${classes})

vtk_module_link(ComputeCurvature PUBLIC DGtal::DGtal ${DGtalLibDependencies})