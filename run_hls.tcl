#*******************************************************************************
# Create a project
open_project -reset proj_ZynqNet

# Add design files
add_files ProcessingElement.cpp
add_files Buffers.cpp
add_files Controller.cpp
# Add test bench & files
add_files -tb TestBench.cpp
add_files -tb data

# Set the top-level function
set_top controller_3x3

# ########################################################
# Create a solution
open_solution -reset solution1
# Define technology and clock rate
set_part  {xc7k325tffg900-2}
create_clock -period 150MHz
exit

# XSIP watermark, do not delete 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
