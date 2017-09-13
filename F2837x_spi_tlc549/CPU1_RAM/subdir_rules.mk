################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Christian/Programme/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --include_path="D:/Christian/Programme/ti/Workspaces/F2837xD/F2837x_spi_tlc549/device" --include_path="D:/Christian/Programme/ti/c2000/C2000Ware_1_00_01_00/driverlib/f2837xd/driverlib" --include_path="D:/Christian/Programme/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --define=DEBUG --define=_LAUNCHXL_F28379D --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


