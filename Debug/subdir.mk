################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../arkanoPiLib.c \
../arkanoPi_1.c \
../kbhit.c 

OBJS += \
./arkanoPiLib.o \
./arkanoPi_1.o \
./kbhit.o 

C_DEPS += \
./arkanoPiLib.d \
./arkanoPi_1.d \
./kbhit.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"C:\SysGCC\Raspberry/include" -I"C:\SysGCC\Raspberry\include\wiringPi" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


