#include <vtkObjectFactory.h>

#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "DGtalComputeIICurvature.h"

#include "../utils/DGtalVTKSurface.h"
#include "../utils/VTKToDGtalVTKImage.h"
#include "../utils/DGtalVTKAbstractContainerToVTK.h"


vtkStandardNewMacro(DGtalComputeIICurvature);

//----------------------------------------------------------------------------
DGtalComputeIICurvature::DGtalComputeIICurvature() 
{
  this->SetNumberOfInputPorts(1);
}

//----------------------------------------------------------------------------
DGtalComputeIICurvature::~DGtalComputeIICurvature() = default;

//----------------------------------------------------------------------------
void DGtalComputeIICurvature::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void DGtalComputeIICurvature::SetCurvatureType(int type)
{
    curvatureType = static_cast<CurvatureType>(type);
    this->Modified();
}


//----------------------------------------------------------------------------
int DGtalComputeIICurvature::RequestData(vtkInformation *request,
                                   vtkInformationVector **inputVectors,
                                   vtkInformationVector *outputVector)
{
  auto params = SH3::defaultParameters() | SHG3::defaultParameters();
  
  DGtalVTKImage image = GetImageFromVtkInformation(inputVectors[0]->GetInformationObject(0));
  DGtalVTKSurface surface(image); 

  ScalarData curvature;
  switch(curvatureType)
  {
    case CurvatureType::MEAN_CURVATURE:
    {
      curvature.data = SHG3::getIIMeanCurvatures( surface.GetImage(), surface.GetSurfelRange(), params );
      curvature.name = "IIMeanCurvature";
      break;
    }
    case CurvatureType::GAUSSIAN_CURVATURE:
    {
      curvature.data = SHG3::getIIGaussianCurvatures( surface.GetImage(), surface.GetSurfelRange(), params );
      curvature.name = "IIGaussianCurvature";
      break;
    }
    default:
    {
      vtkVLog(vtkLogger::VERBOSITY_ERROR, "Unknown curvature type");
      break;
    }      
  };

  vtkSmartPointer<vtkUnstructuredGrid> grid = GetVtkDataSetFromAbstractContainer(&surface, curvature);
  outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), grid);
  return 1;
}
