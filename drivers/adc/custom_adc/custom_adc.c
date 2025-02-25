/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT zephyr_custom_adc

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(custom_adc, CONFIG_SENSOR_LOG_LEVEL);

struct custom_adc_data {
	int state;
};

struct custom_adc_config {
	struct gpio_dt_spec input;
};

static int custom_adc_sample_fetch(const struct device *dev,
				      enum sensor_channel chan)
{
	const struct custom_adc_config *config = dev->config;
	struct custom_adc_data *data = dev->data;

	data->state = gpio_pin_get_dt(&config->input);

	return 0;
}

static int custom_adc_channel_get(const struct device *dev,
				     enum sensor_channel chan,
				     struct sensor_value *val)
{
	struct custom_adc_data *data = dev->data;

	if (chan != SENSOR_CHAN_PROX) {
		return -ENOTSUP;
	}

	val->val1 = data->state;

	return 0;
}

static DEVICE_API(sensor, custom_adc_api) = {
	.sample_fetch = &custom_adc_sample_fetch,
	.channel_get = &custom_adc_channel_get,
};

static int custom_adc_init(const struct device *dev)
{
	const struct custom_adc_config *config = dev->config;

	int ret;

	if (!device_is_ready(config->input.port)) {
		LOG_ERR("Input GPIO not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&config->input, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Could not configure input GPIO (%d)", ret);
		return ret;
	}

	return 0;
}

#define EXAMPLE_SENSOR_INIT(i)						       \
	static struct custom_adc_data example_sensor_data_##i;	       \
									       \
	static const struct custom_adc_config example_sensor_config_##i = {\
		.input = GPIO_DT_SPEC_INST_GET(i, input_gpios),		       \
	};								       \
									       \
	DEVICE_DT_INST_DEFINE(i, custom_adc_init, NULL,		       \
			      &custom_adc_data_##i,			       \
			      &custom_adc_config_##i, POST_KERNEL,	       \
			      CONFIG_SENSOR_INIT_PRIORITY, &custom_adc_api);

DT_INST_FOREACH_STATUS_OKAY(EXAMPLE_SENSOR_INIT)
