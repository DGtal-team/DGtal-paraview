#ifndef vtkMyElevationFilter_h
#define vtkMyElevationFilter_h

#include <vtkAlgorithm.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "ComputeCurvatureModule.h" // for export macro

class COMPUTECURVATURE_EXPORT DGtalComputeIICurvature : public vtkUnstructuredGridAlgorithm
{
public:
  enum class CurvatureType
  {
      MEAN_CURVATURE = 1, 
      GAUSSIAN_CURVATURE = 2
  };

  static DGtalComputeIICurvature* New();
  vtkTypeMacro(DGtalComputeIICurvature, vtkUnstructuredGridAlgorithm);
  
  int RequestData(vtkInformation *request,
                  vtkInformationVector **inputVectors,
                  vtkInformationVector *outputVector);

  // Not using enum for now

  void SetCurvatureType(int type);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  CurvatureType curvatureType = CurvatureType::MEAN_CURVATURE;


protected:
  DGtalComputeIICurvature();
  ~DGtalComputeIICurvature();

private:
  DGtalComputeIICurvature(const DGtalComputeIICurvature&) = delete;
  void operator=(const DGtalComputeIICurvature&) = delete;
};

#endif
