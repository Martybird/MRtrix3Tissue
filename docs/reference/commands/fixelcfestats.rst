.. _fixelcfestats:

fixelcfestats
===========

Synopsis
--------

::

    fixelcfestats [ options ]  input template design contrast tracks output

-  *input*: a text file listing the file names of the input fixel images
-  *template*: the fixel mask used to define fixels of interest. This can be generated by thresholding the group average AFD fixel image.
-  *design*: the design matrix. Note that a column of 1's will need to be added for correlations.
-  *contrast*: the contrast vector, specified as a single row of weights
-  *tracks*: the tracks used to determine fixel-fixel connectivity
-  *output*: the filename prefix for all output.

Description
-----------

Fixel-based analysis using connectivity-based fixel enhancement and non-parametric permutation testing.

Options
-------

-  **-notest** don't perform permutation testing and only output population statistics (effect size, stdev etc)

-  **-negative** automatically test the negative (opposite) contrast. By computing the opposite contrast simultaneously the computation time is reduced.

-  **-nperms num** the number of permutations (default: 5000).

-  **-cfe_dh value** the height increment used in the cfe integration (default: 0.1)

-  **-cfe_e value** cfe extent exponent (default: 2)

-  **-cfe_h value** cfe height exponent (default: 3)

-  **-cfe_c value** cfe connectivity exponent (default: 0.5)

-  **-angle value** the max angle threshold for computing inter-subject fixel correspondence (Default: 30 degrees)

-  **-connectivity threshold** a threshold to define the required fraction of shared connections to be included in the neighbourhood (default: 0.01)

-  **-smooth FWHM** smooth the fixel value along the fibre tracts using a Gaussian kernel with the supplied FWHM (default: 10mm)

-  **-nonstationary** do adjustment for non-stationarity

-  **-nperms_nonstationary num** the number of permutations used when precomputing the empirical statistic image for nonstationary correction (Default: 5000)

Standard options
^^^^^^^^^^^^^^^^

-  **-info** display information messages.

-  **-quiet** do not display information messages or progress status.

-  **-debug** display debugging messages.

-  **-force** force overwrite of output files. Caution: Using the same file as input and output might cause unexpected behaviour.

-  **-nthreads number** use this number of threads in multi-threaded applications (set to 0 to disable multi-threading)

-  **-failonwarn** terminate program if a warning is produced

-  **-help** display this information page and exit.

-  **-version** display version information and exit.

References
^^^^^^^^^^

Raffelt, D.; Smith, RE.; Ridgway, GR.; Tournier, JD.; Vaughan, DN.; Rose, S.; Henderson, R.; Connelly, A.Connectivity-based fixel enhancement: Whole-brain statistical analysis of diffusion MRI measures in the presence of crossing fibres. Neuroimage, 2015, 15(117):40-55

* If using the -nonstationary option: Salimi-Khorshidi, G. Smith, S.M. Nichols, T.E. Adjusting the effect of nonstationarity in cluster-based and TFCE inference. NeuroImage, 2011, 54(3), 2006-19

--------------



**Author:** David Raffelt (david.raffelt@florey.edu.au)

**Copyright:** Copyright (c) 2008-2016 the MRtrix3 contributors

This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/

MRtrix is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

For more details, see www.mrtrix.org
