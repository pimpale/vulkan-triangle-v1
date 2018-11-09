################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/error_methods.c \
../src/glfw_methods.c \
../src/main.c \
../src/util_methods.c \
../src/vulkan_methods.c 

OBJS += \
./src/error_methods.o \
./src/glfw_methods.o \
./src/main.o \
./src/util_methods.o \
./src/vulkan_methods.o 

C_DEPS += \
./src/error_methods.d \
./src/glfw_methods.d \
./src/main.d \
./src/util_methods.d \
./src/vulkan_methods.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	tcc -std=c11 -I/usr/include/GLFW -I/usr/include/vulkan -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MF"$(@:%.o=%.d)"  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


