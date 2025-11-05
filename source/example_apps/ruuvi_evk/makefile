SRCS += $(APP_SRCS_PATH)LIS2DH12_driver/lis2dh12_STdC/driver/lis2dh12_reg.c
SRCS += $(APP_SRCS_PATH)lis2dh12_wrapper.c

#SRCS += $(APP_SRCS_PATH)dps310_driver/dps310.c
#SRCS += $(APP_SRCS_PATH)dps310_wrapper.c

SRCS += $(APP_SRCS_PATH)SHT_driver/embedded-common/sensirion_common.c
SRCS += $(APP_SRCS_PATH)shtc1.c
SRCS += $(APP_SRCS_PATH)sht_wrapper.c

SRCS += $(APP_SRCS_PATH)app_config.c
SRCS += $(APP_SRCS_PATH)format_data.c

INCLUDES += -I$(APP_SRCS_PATH)LIS2DH12_driver/lis2dh12_STdC/driver/
INCLUDES += -I$(APP_SRCS_PATH)SHT_driver/shtc1/
INCLUDES += -I$(APP_SRCS_PATH)SHT_driver/embedded-common/
INCLUDES += -I$(APP_SRCS_PATH)SHT_driver/sht-common/
#INCLUDES += -I$(APP_SRCS_PATH)dps310_driver/

# This application use HAL for communication
HAL_SPI=yes
HAL_I2C=yes
HAL_GPIO=yes

# This application use HAL for buttons and leds
#HAL_BUTTON=yes
HAL_LED=yes

# Use App Scheduler and configure it
APP_SCHEDULER=yes
APP_SCHEDULER_TASKS=2

# Use RTT debug print
APP_PRINTING=yes

# Define a specific target to clone the Bosh and ST Libraries
# DEFAULT_GOAL is define as this target will be declared as the first one in global makefile
# and would become the default target without explicitely defining it
.DEFAULT_GOAL := all
$(SRCS_PATH)LIS2DH12_driver/lis2dh12_STdC/driver/lis2dh12_reg.c:
	$(shell git clone https://github.com/STMicroelectronics/STMems_Standard_C_drivers.git $(SRCS_PATH)LIS2DH12_driver \
		&& cd $(SRCS_PATH)LIS2DH12_driver && git checkout v1.01 -b v1.01)

$(SRCS_PATH)sht_wrapper.c: $(APP_SRCS_PATH)SHT_driver/embedded-common/sensirion_common.c
$(SRCS_PATH)shtc1.c: $(APP_SRCS_PATH)SHT_driver/embedded-common/sensirion_common.c
$(SRCS_PATH)SHT_driver/embedded-common/sensirion_common.c:
	@echo "Cloning SHT_driver with submodules..."
	@git clone --recurse-submodules https://github.com/Sensirion/embedded-sht.git $(SRCS_PATH)SHT_driver && \
	cd $(SRCS_PATH)SHT_driver && \
	git checkout 5.3.0 -b v5.3.0
