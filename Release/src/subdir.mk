################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/error_handle.c \
../src/main.c \
../src/vulkan_methods.c 

OBJS += \
./src/error_handle.o \
./src/main.o \
./src/vulkan_methods.o 

C_DEPS += \
./src/error_handle.d \
./src/main.d \
./src/vulkan_methods.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


