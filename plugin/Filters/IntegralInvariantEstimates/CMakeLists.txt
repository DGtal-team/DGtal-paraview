set(classes
  DGtalIIEstimators)

vtk_module_add_module(IntegralInvariantEstimates
  CLASSES ${classes})

vtk_module_link(IntegralInvariantEstimates PUBLIC DGtal::DGtal ${DGtalLibDependencies})
