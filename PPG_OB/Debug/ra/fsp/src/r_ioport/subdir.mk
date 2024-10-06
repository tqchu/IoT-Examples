################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_ioport/r_ioport.c 

C_DEPS += \
./ra/fsp/src/r_ioport/r_ioport.d 

OBJS += \
./ra/fsp/src/r_ioport/r_ioport.o 

SREC += \
PPG_OB.srec 

MAP += \
PPG_OB.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_ioport/%.o: ../ra/fsp/src/r_ioport/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/src" -I"." -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/ra/fsp/inc" -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/ra/fsp/inc/api" -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/ra/fsp/inc/instances" -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/ra_gen" -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/ra_cfg/fsp_cfg/bsp" -I"/home/truongchu/Academic/HK9/IoT/PPG_OB/ra_cfg/fsp_cfg" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

