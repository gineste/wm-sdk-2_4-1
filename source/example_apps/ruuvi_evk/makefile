SRCS += $(APP_SRCS_PATH)LIS2DH12_driver/lis2dh12_STdC/driver/lis2dh12_reg.c
SRCS += $(APP_SRCS_PATH)lis2dh12_wrapper.c
SRCS += $(APP_SRCS_PATH)app_config.c
SRCS += $(APP_SRCS_PATH)format_data.c
INCLUDES += -I$(APP_SRCS_PATH)LIS2DH12_driver/lis2dh12_STdC/driver/

HAL_SPI=yes
HAL_GPIO=yes

# Use App Scheduler and configure it
APP_SCHEDULER=yes
APP_SCHEDULER_TASKS=4

# Define a specific target to clone the Bosh and ST Libraries
# DEFAULT_GOAL is define as this target will be declared as the first one in global makefile
# and would become the default target without explicitely defining it
.DEFAULT_GOAL := all
$(SRCS_PATH)LIS2DH12_driver/lis2dh12_STdC/driver/lis2dh12_reg.c:
	$(shell git clone https://github.com/STMicroelectronics/STMems_Standard_C_drivers.git $(SRCS_PATH)LIS2DH12_driver \
		&& cd $(SRCS_PATH)LIS2DH12_driver && git checkout v1.01 -b v1.01)
