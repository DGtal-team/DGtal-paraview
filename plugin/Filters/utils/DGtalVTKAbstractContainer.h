#pragma once


#include <iterator>
#include <memory>
#include <array>
#include <cmath>

class DGtalVTKAbstractContainer
{
public:
	void SetBounds(const DGtalVTKAbstractContainer& other)
	{
		nbCells = other.nbCells;
		cellSize = other.cellSize;
		imagePos = other.imagePos;
		imageSize = other.imageSize;
		domainSize = other.domainSize;
	}

	void SetBounds(
		const double imageBounds[6], 
		const double cellBounds[6], 
		const unsigned int nbCells
	)
	{
		imagePos[0] = imageBounds[0];
		imagePos[1] = imageBounds[2];
		imagePos[2] = imageBounds[4];

		imageSize[0] = imageBounds[1] - imageBounds[0];
		imageSize[1] = imageBounds[3] - imageBounds[2];
		imageSize[2] = imageBounds[5] - imageBounds[4]; 

		cellSize[0] = cellBounds[1] - cellBounds[0];
		cellSize[1] = cellBounds[3] - cellBounds[2];
		cellSize[2] = cellBounds[5] - cellBounds[4];
		
		domainSize[0] = std::round(imageSize[0] / cellSize[0]) + 1;
		domainSize[1] = std::round(imageSize[1] / cellSize[1]) + 1;
		domainSize[2] = std::round(imageSize[2] / cellSize[2]) + 1;

		this->nbCells = nbCells;
	}

	unsigned int GetCellCount() const 
	{
		return nbCells;
	}

	const unsigned int* GetDomainSize() const
	{
		return domainSize.data();
	}

	const double* GetImageSize() const
	{
		return imageSize.data();
	}

	const double* GetImagePos() const
	{
		return imagePos.data();
	}

	const double* GetCellSize() const
	{
		return cellSize.data();
	}

	virtual ~DGtalVTKAbstractContainer()
	{ }
public:
	struct DgtalToVtkIterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = const double*;
		using pointer = const double*;
		using reference = const double*;
		using iterator_category = std::forward_iterator_tag;

		struct IteratorImpl 
		{
			virtual void Increment() = 0;
			virtual void Advance() = 0;
			virtual void FillCellCoordinates(double* ) const = 0;
			virtual bool Compare(const IteratorImpl* other) const = 0;

			virtual ~IteratorImpl() {};
		};


		DgtalToVtkIterator(IteratorImpl* it) : 
			it(it)
		{ 
			it->Advance();
		}

		DgtalToVtkIterator& operator++() 
		{
			it->Increment();
			it->Advance();
			return *this;
		}

		DgtalToVtkIterator operator++(int)
		{
			DgtalToVtkIterator it = *this;
			++(*this);
			return it;
		}

		bool operator==(const DgtalToVtkIterator& other) const 
		{
			return it->Compare(other.it);
		}

		bool operator!=(const DgtalToVtkIterator& other) const
		{
			return !(*this == other);
		}

		const double* operator*()
		{
			it->FillCellCoordinates(currentCell);
			return currentCell;
		}

		virtual ~DgtalToVtkIterator()
		{
			delete it;
		}
	private:
		IteratorImpl* it;
		double currentCell[3];
	};

	virtual DgtalToVtkIterator begin() const = 0;
	virtual DgtalToVtkIterator end()   const = 0;
protected:
	void AdvanceIt();
	void FillCellCoordinates(double* cell);
protected:
	unsigned int nbCells = 0;

	std::array<unsigned int, 3> domainSize = {0, 0, 0};
	std::array<double, 3> imageSize = {0., 0., 0.};
	std::array<double, 3> imagePos  = {0., 0., 0.};
	std::array<double, 3> cellSize  = {0., 0., 0.};
};