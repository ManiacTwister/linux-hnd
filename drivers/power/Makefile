power_supply-objs := power_supply_core.o

ifeq ($(CONFIG_SYSFS),y)
power_supply-objs += power_supply_sysfs.o
endif

ifeq ($(CONFIG_LEDS_TRIGGERS),y)
power_supply-objs += power_supply_leds.o
endif

ifeq ($(CONFIG_POWER_SUPPLY_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG
endif

obj-$(CONFIG_POWER_SUPPLY)         += power_supply.o

obj-$(CONFIG_PDA_POWER)            += pda_power.o
obj-$(CONFIG_APM_POWER)            += apm_power.o

obj-$(CONFIG_BATTERY_DS2760)       += ds2760_battery.o
obj-$(CONFIG_BATTERY_PALM)         += palm_battery.o
obj-$(CONFIG_BATTERY_PMU)          += pmu_battery.o
obj-$(CONFIG_BATTERY_OLPC)         += olpc_battery.o

# drivers below are not in battery2-2.6 tree

obj-$(CONFIG_ADC_BATTERY)          += adc_battery.o
obj-$(CONFIG_IPAQ_MICRO_BATTERY)   += micro_battery.o
obj-$(CONFIG_MCP_UCB1x00_SIMPAD_BATTERY) += simpad-battery.o
