/*
Copyright (C) 2010 David Doria, daviddoria@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef IMAGEGRAPHCUT_H
#define IMAGEGRAPHCUT_H

// VTK
#include <vtkSmartPointer.h>
class vtkPolyData;

#include "Mask/Mask.h"

// ITK
#include "itkImage.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"
#include "itkListSample.h"

// STL
#include <vector>

// Custom
#include "Types.h"

// Kolmogorov's code
#include "graph.h"
typedef Graph GraphType;

// This is a special type to keep track of the graph node labels
typedef itk::Image<void*, 2> NodeImageType;

typedef itk::Statistics::Histogram< float,
        itk::Statistics::DenseFrequencyContainer2 > HistogramType;


class ImageGraphCut
{
public:
  //void SetSources(vtkPolyData* sources);
  //void SetSinks(vtkPolyData* sinks);

  // Several initializations are done here
  void SetImage(ImageType* const image);

  ImageType* GetImage();

  // Create and cut the graph (The main driver function)
  void PerformSegmentation();

  // Return a list of the selected (via scribbling) pixels
  std::vector<itk::Index<2> > GetSources();
  std::vector<itk::Index<2> > GetSinks();

  // Set the selected (via scribbling) pixels
  void SetSources(vtkPolyData* const sources);
  void SetSinks(vtkPolyData* const sinks);

  void SetSources(const std::vector<itk::Index<2> >& sources);
  void SetSinks(const std::vector<itk::Index<2> >& sinks);

  // Get the output of the segmentation
  Mask* GetSegmentMask();

  // Set the weight between the regional and boundary terms
  void SetLambda(const float);

  // Set the weight of the RGB components of the pixels vs the rest of the components
  void SetRGBWeight(const float);

  // Set the number of bins per dimension of the foreground and background histograms
  void SetNumberOfHistogramBins(const int);

protected:

  /** A Kolmogorov graph object */
  GraphType* Graph;

  /** The output segmentation */
  Mask::Pointer SegmentMask;

  /** User specified foreground points */
  std::vector<itk::Index<2> > Sources;

  /** User specified background points */
  std::vector<itk::Index<2> > Sinks;

  /** The weighting between unary and binary terms */
  float Lambda;

  /** The number of bins per dimension of the foreground and background histograms */
  int NumberOfHistogramBins;

  /** An image which keeps tracks of the mapping between pixel index and graph node id */
  NodeImageType::Pointer NodeImage;

  /** Determine if a number is NaN */
  bool IsNaN(const double a);

  float RGBWeight;

  // Typedefs
  typedef itk::Statistics::ListSample<PixelType> SampleType;
  typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;

  /** Create the histograms from the users selections */
  void CreateSamples();

  /** Estimate the "camera noise" */
  double ComputeNoise();

  /** Create a Kolmogorov graph structure from the image and selections */
  void CreateGraph();

  /** Perform the s-t min cut */
  void CutGraph();

  float PixelDifference(const PixelType& a, const PixelType& b);

  // Member variables */
  SampleType::Pointer ForegroundSample;
  SampleType::Pointer BackgroundSample;

  const HistogramType* ForegroundHistogram;
  const HistogramType* BackgroundHistogram;

  SampleToHistogramFilterType::Pointer ForegroundHistogramFilter;
  SampleToHistogramFilterType::Pointer BackgroundHistogramFilter;

  /** The image to be segmented */
  ImageType::Pointer Image;

};

#endif