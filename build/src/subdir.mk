################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/errors.c \
../src/main.c \
../src/utils.c \
../src/vulkan_helper.c \
../src/vulkan_utils.c 

OBJS += \
./src/errors.o \
./src/main.o \
./src/utils.o \
./src/vulkan_helper.o \
./src/vulkan_utils.o 

C_DEPS += \
./src/errors.d \
./src/main.d \
./src/utils.d \
./src/vulkan_helper.d \
./src/vulkan_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c11 -I/usr/include/GLFW -I/usr/include/vulkan -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


