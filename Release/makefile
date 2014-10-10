################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := G:/MyDocuments/Projects/01_SW/ti/CCS_6/ccsv6/tools/compiler/arm_5.1.8

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./main.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./utils/uartstdio.obj" \
"./driverlib/UART/uart.obj" \
"./driverlib/SysCtl/sysctl.obj" \
"./driverlib/SPI/ssi.obj" \
"./driverlib/Interrupt/interrupt.obj" \
"./driverlib/GPIO/gpio.obj" \
"./driverlib/CPU/cpu.obj" \
"../tm4c123gh6pm.cmd" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include utils/subdir_vars.mk
-include driverlib/UART/subdir_vars.mk
-include driverlib/SysCtl/subdir_vars.mk
-include driverlib/SPI/subdir_vars.mk
-include driverlib/Interrupt/subdir_vars.mk
-include driverlib/GPIO/subdir_vars.mk
-include driverlib/CPU/subdir_vars.mk
-include subdir_rules.mk
-include utils/subdir_rules.mk
-include driverlib/UART/subdir_rules.mk
-include driverlib/SysCtl/subdir_rules.mk
-include driverlib/SPI/subdir_rules.mk
-include driverlib/Interrupt/subdir_rules.mk
-include driverlib/GPIO/subdir_rules.mk
-include driverlib/CPU/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Control_Board.out \

EXE_OUTPUTS__QUOTED += \
"Control_Board.out" \

BIN_OUTPUTS += \
Control_Board.hex \

BIN_OUTPUTS__QUOTED += \
"Control_Board.hex" \


# All Target
all: Control_Board.out

# Tool invocations
Control_Board.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"G:/MyDocuments/Projects/01_SW/ti/CCS_6/ccsv6/tools/compiler/arm_5.1.8/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off -z -m"Control_Board.map" --heap_size=0 --stack_size=512 -i"G:/MyDocuments/Projects/01_SW/ti/CCS_6/ccsv6/tools/compiler/arm_5.1.8/lib" -i"G:/MyDocuments/Projects/01_SW/ti/CCS_6/ccsv6/tools/compiler/arm_5.1.8/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="Control_Board_linkInfo.xml" --rom_model -o "Control_Board.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

Control_Board.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"G:/MyDocuments/Projects/01_SW/ti/CCS_6/ccsv6/tools/compiler/arm_5.1.8/bin/armhex"  -o "Control_Board.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "main.pp" "tm4c123gh6pm_startup_ccs.pp" "utils\uartstdio.pp" "driverlib\UART\uart.pp" "driverlib\SysCtl\sysctl.pp" "driverlib\SPI\ssi.pp" "driverlib\Interrupt\interrupt.pp" "driverlib\GPIO\gpio.pp" "driverlib\CPU\cpu.pp" 
	-$(RM) "main.obj" "tm4c123gh6pm_startup_ccs.obj" "utils\uartstdio.obj" "driverlib\UART\uart.obj" "driverlib\SysCtl\sysctl.obj" "driverlib\SPI\ssi.obj" "driverlib\Interrupt\interrupt.obj" "driverlib\GPIO\gpio.obj" "driverlib\CPU\cpu.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

