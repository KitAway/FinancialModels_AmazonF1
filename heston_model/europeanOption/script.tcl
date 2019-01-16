open_project -reset hls_heston
set_top hestonEuro
add_files hestonEuro.cpp -cflags "-I../../common -I."
#add_files -tb main_tb.cpp -cflags "-Wno-unknown-pragmas -Wno-unknown-pragmas"
open_solution -reset "zynq-z2"
set_part {xc7z020clg400-1}
create_clock -period 4 -name default
config_schedule -effort medium -relax_ii_for_timing 
config_interface   -m_axi_addr64 -m_axi_offset off -register_io off 
#csim_design
csynth_design
#cosim_design -trace_level all
export_design -rtl verilog -format ip_catalog
