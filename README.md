# FPGA-ZynqNet
FPGA-based CNN accelerator developed by Vivado HLS 
ZynqNet(https://github.com/dgschwend/zynqnet) is a Convolution Neural Network designed for ImageNet classification which is similar to SqueezeNet-V1.1

Quantization: 8-bit dynamic fixed point
Environment: Vivado HLS 2016.4
Target Device: xc7k325tffg900-2

Usage:
    1.Open Vivado HLS Command Prompt
    2.Change to source file direction
    3.Type "vivado_hls -f run_hls.tcl" to create HLS project
    4.Type "vivado_hls -p proj_ZynqNet" to open HLS project
