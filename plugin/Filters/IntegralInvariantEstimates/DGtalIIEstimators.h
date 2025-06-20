#ifndef vtkMyElevationFilter_h
#define vtkMyElevationFilter_h

#include <vtkAlgorithm.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "IntegralInvariantEstimatesModule.h" // for export macro

class INTEGRALINVARIANTESTIMATES_EXPORT DGtalIIEstimators : public vtkUnstructuredGridAlgorithm
{
public:
  static DGtalIIEstimators* New();
  vtkTypeMacro(DGtalIIEstimators, vtkUnstructuredGridAlgorithm);
  
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
  DGtalIIEstimators();
  ~DGtalIIEstimators();

private:
  DGtalIIEstimators(const DGtalIIEstimators&) = delete;
  void operator=(const DGtalIIEstimators&) = delete;
};

#endif
