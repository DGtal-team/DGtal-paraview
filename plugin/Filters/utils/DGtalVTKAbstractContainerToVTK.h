#pragma once

#include "DGtalVTKAbstractContainer.h"

#include <vtkUnstructuredGrid.h>
#include <vtkTypeInt64Array.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPoints.h>
#include <vtkLogger.h>

#if VTK_TYPE_INT64 == VTK_LONG
using VtkInt64 = long int;
#elif VTK_TYPE_INT64 == VTK_LONG_LONG
using VtkInt64 = long long int;
#else
using VtkInt64 = DGtal::int64_t;
#endif

template<typename T>
struct ElementData 
{
    std::string name;
    std::vector<T> data;
};

using ScalarData = ElementData<double>;
using VectorData = ElementData<std::array<double, 3>>;

inline void FillCube(const double* center, const double* cellSize, double* location)
{
    static constexpr double factors[24] = {
        -0.5, -0.5, -0.5, 
         0.5, -0.5, -0.5, 
        -0.5,  0.5, -0.5, 
         0.5,  0.5, -0.5, 
        -0.5, -0.5,  0.5, 
         0.5, -0.5,  0.5, 
        -0.5,  0.5,  0.5, 
         0.5,  0.5,  0.5
    };

    for (unsigned int i = 0; i < 8; i++)
    {
        for (unsigned int k = 0; k < 3; k++)
        {
            location[i * 3 + k] = center[k] + factors[i * 3 + k] * cellSize[k];
        }
    }
}

inline vtkSmartPointer<vtkPoints> PointsFromDGtalVTKAbstractContainer(const DGtalVTKAbstractContainer* container)
{
    const unsigned int dim = 3;
    const unsigned int pointsPerCube = 8;
    const unsigned int nbPoints = pointsPerCube * container->GetCellCount(); 

    vtkDoubleArray* vtkPointsArray = vtkDoubleArray::New();
    vtkPointsArray->SetNumberOfComponents(dim); 
    
    double* pointsPtr = ((vtkDoubleArray*)vtkPointsArray)->WritePointer(0, nbPoints * dim);
    
    unsigned int ptCount = 0;
    for (auto it = container->begin(); it != container->end(); ++it)
    {
        if (ptCount == nbPoints)
        {
            vtkVLog(vtkLogger::VERBOSITY_ERROR, "More points in the shape than the nb of cells !");
            std::cout << ptCount << std::endl;
            return nullptr;
        }

        FillCube(*it, container->GetCellSize(), pointsPtr + ptCount * pointsPerCube * dim);
        ptCount++;
    }

    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetData(vtkPointsArray);

    return pts;
}

inline vtkSmartPointer<vtkCellArray> CellsFromDGtalVTKAbstractContainer(const DGtalVTKAbstractContainer* container)
{
    const unsigned int dim = 3;
    const unsigned int pointsPerCube = 8;
    const unsigned int nbPoints = pointsPerCube * container->GetCellCount(); 

    // Cell offsets (cube is 8 points -> 0, 8, 16, ...)
    // +1 so that end is past last point
    vtkSmartPointer<vtkTypeInt64Array> vtkOffsetArray = vtkSmartPointer<vtkTypeInt64Array>::New();
    vtkOffsetArray->SetNumberOfComponents(1); 
    
    VtkInt64* offsetPtr = ((vtkTypeInt64Array*)vtkOffsetArray.Get())->WritePointer(0, container->GetCellCount() + 1);
    for (unsigned int i = 0; i < container->GetCellCount() + 1; i++) 
        offsetPtr[i] = i * pointsPerCube;

    // Cell connectivity (points of same cell are next to each other -> 0, 1, 2, ...)
    vtkSmartPointer<vtkTypeInt64Array> vtkConnArray = vtkSmartPointer<vtkTypeInt64Array>::New();
    vtkConnArray->SetNumberOfComponents(1); 
    
    VtkInt64* connPtr = ((vtkTypeInt64Array*)vtkConnArray.Get())->WritePointer(0, nbPoints);
    for (unsigned int i = 0; i < nbPoints; i++) 
        connPtr[i] = i;

    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    cells->SetData(vtkOffsetArray.Get(), vtkConnArray.Get()); // TODO: check for success ? 
    cells->Register(nullptr);
    return cells;
}

inline std::vector<int> CellTypesFromDGtalVTKAbstractContainer(const DGtalVTKAbstractContainer* container)
{
    std::vector<int> types(container->GetCellCount(), VTK_VOXEL);
    return types;
}

inline vtkDataArray* CellDataFromDGtalVTKAbstractContainer(const DGtalVTKAbstractContainer* container, const ScalarData& data)
{
    if (data.data.size() != container->GetCellCount()) return nullptr;

    vtkDoubleArray* vtkScalarArray = vtkDoubleArray::New();
    vtkScalarArray->SetNumberOfComponents(1); 
    
    double* scalarPtr = ((vtkDoubleArray*)vtkScalarArray)->WritePointer(0, data.data.size());
    for (unsigned int i = 0; i < container->GetCellCount(); i++)
        scalarPtr[i] = data.data[i];

    vtkDataArray* vtkData = vtkArrayDownCast<vtkDataArray>(vtkScalarArray);
    vtkData->SetName(data.name.c_str());

    return vtkData;
}

inline vtkDataArray* CellVectorFromDGtalVTKAbstractContainer(const DGtalVTKAbstractContainer* container, const VectorData& data)
{
    if (data.data.size() != container->GetCellCount()) return nullptr;

    vtkDoubleArray* vtkScalarArray = vtkDoubleArray::New();
    vtkScalarArray->SetNumberOfComponents(3); 
    
    double* scalarPtr = ((vtkDoubleArray*)vtkScalarArray)->WritePointer(0, 3 * data.data.size());
    for (unsigned int i = 0; i < container->GetCellCount(); i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            scalarPtr[j + i * 3] = data.data[i][j];
        }
    }

    vtkDataArray* vtkData = vtkArrayDownCast<vtkDataArray>(vtkScalarArray);
    vtkData->SetName(data.name.c_str());

    return vtkData;
}

inline vtkSmartPointer<vtkUnstructuredGrid> GetVtkDataSetFromAbstractContainer(
    const DGtalVTKAbstractContainer* container, 
    const std::vector<ScalarData>& voxelScalarData, 
    const std::vector<VectorData>& voxelVectorData, 
    const VectorData& normals = {}
)
{
    auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    
    vtkSmartPointer<vtkPoints> pts = PointsFromDGtalVTKAbstractContainer(container);
    grid->SetPoints(pts.Get());

    vtkSmartPointer<vtkCellArray> cells = CellsFromDGtalVTKAbstractContainer(container);
    std::vector<int> types = CellTypesFromDGtalVTKAbstractContainer(container);

    grid->SetCells(types.data(), cells.Get());

    // Add data to grid
    for (size_t i = 0; i < voxelScalarData.size(); ++i)
    {
        vtkDataArray* data = CellDataFromDGtalVTKAbstractContainer(container, voxelScalarData[i]);
        if (data != nullptr)
        {
            grid->GetCellData()->AddArray(data);
            data->Delete();
        }
    }

    for (size_t i = 0; i < voxelVectorData.size(); ++i)
    {
        vtkDataArray* data = CellVectorFromDGtalVTKAbstractContainer(container, voxelVectorData[i]);
        if (data != nullptr)
        {
            grid->GetCellData()->AddArray(data);
            data->Delete();
        }
    }

    // Set normals if any
    if (normals.data.size() != 0)
    {
        std::cout << "Setting normals" << std::endl;
        vtkDataArray* data = CellVectorFromDGtalVTKAbstractContainer(container, normals);
        if (data != nullptr)
        {
            grid->GetCellData()->SetNormals(data);
            data->Delete();
        }
    }
    return grid;
}
