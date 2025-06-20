#ifndef vtkMyElevationFilter_h
#define vtkMyElevationFilter_h

#include <vtkAlgorithm.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "MeshingModule.h" // for export macro

class MESHING_EXPORT DGtalDigitalSurfaceToMesh : public vtkUnstructuredGridAlgorithm
{
public:
  static DGtalDigitalSurfaceToMesh* New();
  vtkTypeMacro(DGtalDigitalSurfaceToMesh, vtkUnstructuredGridAlgorithm);
  
  int FillOutputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation *request,
                  vtkInformationVector **inputVectors,
                  vtkInformationVector *outputVector);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:

protected:
  DGtalDigitalSurfaceToMesh();
  ~DGtalDigitalSurfaceToMesh();

private:
  DGtalDigitalSurfaceToMesh(const DGtalDigitalSurfaceToMesh&) = delete;
  void operator=(const DGtalDigitalSurfaceToMesh&) = delete;
};

#endif