#ifndef vtkMyElevationFilter_h
#define vtkMyElevationFilter_h

#include <vtkAlgorithm.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "ComputeCurvatureModule.h" // for export macro

class COMPUTECURVATURE_EXPORT DGtalComputeIICurvature : public vtkUnstructuredGridAlgorithm
{
public:
  static DGtalComputeIICurvature* New();
  vtkTypeMacro(DGtalComputeIICurvature, vtkUnstructuredGridAlgorithm);
  
  int RequestData(vtkInformation *request,
                  vtkInformationVector **inputVectors,
                  vtkInformationVector *outputVector);

  void SetComputeMean     (bool value) { computeMean      = value; };
  void SetComputeGaussian (bool value) { computeGaussian  = value; };
  void SetComputePrincipal(bool value) { computePrincipal = value; };
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  bool computeMean = false;
  bool computeGaussian = false;
  bool computePrincipal = false;
  

protected:
  DGtalComputeIICurvature();
  ~DGtalComputeIICurvature();

private:
  DGtalComputeIICurvature(const DGtalComputeIICurvature&) = delete;
  void operator=(const DGtalComputeIICurvature&) = delete;
};

#endif
