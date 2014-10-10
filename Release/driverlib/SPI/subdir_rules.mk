################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
driverlib/SPI/ssi.obj: ../driverlib/SPI/ssi.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"G:/MyDocuments/Projects/01_SW/ti/CCS_6/ccsv6/tools/compiler/arm_5.1.8/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="G:/MyDocuments/Projects/01_SW/ti/CCS_6/ccsv6/tools/compiler/arm_5.1.8/include" --include_path="G:/MyDocuments/Projects/90_Repo/TIVA_C/Control_Board" --include_path="G:/MyDocuments/Projects/01_SW/ti/TivaWare_C_Series-2.1.0.12573" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="driverlib/SPI/ssi.pp" --obj_directory="driverlib/SPI" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


