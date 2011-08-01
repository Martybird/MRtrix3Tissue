/*
    Copyright 2008 Brain Research Institute, Melbourne, Australia

    Written by J-Donald Tournier, 27/11/09.

    This file is part of MRtrix.

    MRtrix is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MRtrix is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "app.h"
#include "progressbar.h"
#include "image/buffer.h"
#include "dataset/loop.h"

using namespace MR;

SET_VERSION_DEFAULT;
SET_AUTHOR (NULL);
SET_COPYRIGHT (NULL);

DESCRIPTION = {
  "average image intensities along specified axis.",
  NULL
};

ARGUMENTS = {
  Argument ("input", "the input image.").type_image_in (),
  Argument ("axis", "the axis along which to average.").type_integer (0),
  Argument ("mean", "the output mean image.").type_image_out (),
  Argument()
};


OPTIONS = {
  Option()
};







EXECUTE {
  Image::Header header_in (argument[0]);
  size_t axis = argument[1];

  std::vector<ssize_t> strides (header_in.ndim(), 0);
  strides[axis] = 1;
  Image::Buffer<float> in (header_in, strides);

  Image::Header header_out (header_in);
  header_out.set_datatype (DataType::Float32);
  if (axis == header_in.ndim() - 1) 
    header_out.set_ndim (header_in.ndim()-1);
  else 
    header_out.set_dim (axis, 1);
  header_out.create (argument[2]);
  Image::Voxel<float> out (header_out);

  DataSet::Loop inner (axis, axis+1);
  DataSet::LoopInOrder outer (header_out, "averaging...");

  float N = header_in.dim(axis);
  for (outer.start (out, in); outer.ok(); outer.next (out, in)) {
    float mean = 0.0;
    for (inner.start (in); inner.ok(); inner.next (in)) 
      mean += in.value();
    out.value() = mean / N;
  }
}
