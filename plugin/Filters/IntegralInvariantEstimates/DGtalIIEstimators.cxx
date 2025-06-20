#include <vtkObjectFactory.h>

#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "DGtalIIEstimators.h"

#include "../utils/DGtalVTKSurface.h"
#include "../utils/VTKToDGtalVTKImage.h"
#include "../utils/DGtalVTKAbstractContainerToVTK.h"


vtkStandardNewMacro(DGtalIIEstimators);

//----------------------------------------------------------------------------
DGtalIIEstimators::DGtalIIEstimators() 
{
  this->SetNumberOfInputPorts(1);
}

//----------------------------------------------------------------------------
DGtalIIEstimators::~DGtalIIEstimators() = default;

//----------------------------------------------------------------------------
void DGtalIIEstimators::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int DGtalIIEstimators::RequestData(vtkInformation *request,
                                         vtkInformationVector **inputVectors,
                                         vtkInformationVector *outputVector)
{
  auto params = SH3::defaultParameters() | SHG3::defaultParameters();
  
  DGtalVTKImage image = GetImageFromVtkInformation(inputVectors[0]->GetInformationObject(0));
  DGtalVTKSurface surface(image); 

  std::vector<ScalarData> curvatures;
  std::vector<VectorData> curvaturesDirs;
  VectorData normals;

  if (computeMean) 
  {
    curvatures.push_back({
        "IIMeanCurvature", 
         SHG3::getIIMeanCurvatures( surface.GetImage(), surface.GetSurfelRange(), params )
    });
  }

  if (computeGaussian) 
  {
    curvatures.push_back({
        "IIGaussianCurvature", 
         SHG3::getIIGaussianCurvatures( surface.GetImage(), surface.GetSurfelRange(), params )
    });
  }

  if (computePrincipal) 
  {
    std::vector<double> k1, k2;
    std::vector<std::array<double, 3>> d1, d2;
    
    auto data = SHG3::getIIPrincipalCurvaturesAndDirections(surface.GetImage(), surface.GetSurfelRange(), params);
    
    auto s = data.size();
    k1.resize(s);
    k2.resize(s);
    d1.resize(s);
    d2.resize(s);
    for (size_t i = 0; i < data.size(); ++i) 
    {
      k1[i] = std::get<0>(data[i]);
      k2[i] = std::get<1>(data[i]);
      for (size_t j = 0; j < 3; ++j)
      {
        d1[i][j] = std::get<2>(data[i])[j];
        d2[i][j] = std::get<3>(data[i])[j];
      }
    }
    
    curvatures.push_back({"IIFirstPrincipalCurvature" , std::move(k1)});
    curvatures.push_back({"IISecondPrincipalCurvature", std::move(k2)});
    curvaturesDirs.push_back({"IIFirstPrincipalCurvatureDir" , std::move(d1)});
    curvaturesDirs.push_back({"IISecondPrincipalCurvatureDir", std::move(d2)});
  }

  if (computeNormals)
  {
    auto data = SHG3::getIINormalVectors(surface.GetImage(), surface.GetSurfelRange(), params);

    std::vector<std::array<double, 3>> normalData(data.size());
    for (size_t i = 0; i < data.size(); ++i)
    {
      for (size_t j = 0; j < 3; ++j)
      {
        normalData[i][j] = data[i][j];
      }
    }
    normals.name = "IINormals";
    normals.data = std::move(normalData);

    // Also as a vector so it can be viewed 
    curvaturesDirs.push_back(normals);
  }

  if (curvatures.size() == 0 && normals.data.size() == 0) 
  {
    vtkVLog(vtkLogger::VERBOSITY_ERROR, "Computation produced no output. Please check that at least one of the curvature is selected");
    return 0;
  }

  vtkSmartPointer<vtkUnstructuredGrid> grid = GetVtkDataSetFromAbstractContainer(
      &surface, curvatures, curvaturesDirs, normals
  );
  outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), grid);
  return 1;
}
