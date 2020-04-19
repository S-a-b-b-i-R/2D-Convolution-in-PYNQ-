############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2014 Xilinx Inc. All rights reserved.
############################################################
open_project CONV
set_top HW_2DConv_Mmap_3
add_files define.hpp
add_files buffer.hpp
add_files 2dconv.hpp
add_files 2dconv.cpp
add_files -tb test.cpp
add_files -tb define.hpp
add_files -tb 2dconv.hpp
open_solution "solution1"
set_part {xc7k160tfbg484-2}
create_clock -period 10 -name default
source "./CONV/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
