# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Users\jvand\workspace\AUDIO_Synthesizer\Audio_Synthesizer\AudioProcessing\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Users\jvand\workspace\AUDIO_Synthesizer\Audio_Synthesizer\AudioProcessing\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {AudioProcessing}\
-hw {C:\Users\jvand\Downloads\AudioProcessing2.xsa}\
-out {C:/Users/jvand/workspace/AUDIO_Synthesizer/Audio_Synthesizer}

platform write
domain create -name {standalone_ps7_cortexa9_0} -display-name {standalone_ps7_cortexa9_0} -os {standalone} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {hello_world}
platform generate -domains 
platform active {AudioProcessing}
domain active {zynq_fsbl}
domain active {standalone_ps7_cortexa9_0}
platform generate -quick
platform generate
