set(classes
  DGtalDigitalSurfaceToMesh)

vtk_module_add_module(Meshing
  CLASSES ${classes})

vtk_module_link(Meshing PUBLIC DGtal::DGtal ${DGtalLibDependencies})
