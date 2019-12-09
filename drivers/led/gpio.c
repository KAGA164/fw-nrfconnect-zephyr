/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <kernel.h>
#include <led.h>
#include <gpio.h>
#include <device.h>
#include <sys/util.h>
#include <log/logging.h>

#include "led_context.h"

LOG_MODULE_REGISTER(led_gpio, CONFIG_LEDS_GPIO_LOG_LEVEL);

struct led_gpio_cfg {
	const char *gpio_port;
	u8_t gpio_pin;
	u8_t gpio_polarity;
};

struct led_gpio_data {
	struct device *gpio;
	struct led_gpio_cfg *cfg;
	struct led_data data;
};

static inline struct led_gpio_data *get_dev_data(struct device *dev)
{
	return dev->driver_data;
}

static inline const struct led_gpio_cfg *get_dev_config(struct device *dev)
{
	return dev->config->config_info;
}

static struct led_gpio_data *led_parameters_get(struct device *dev, u32_t led)
{
	const struct led_gpio_cfg *cfg = get_dev_config(dev);
	struct led_gpio_data *data = get_dev_data(dev);

	for (size_t i = 0; i < DT_GPIO_LEDS_LEDS_GPIOS_COUNT; i++) {
		if (cfg[i].gpio_pin == led) {
			return &data[i];
		}
	}

	return NULL;
}

static int gpio_led_on(struct device *dev, u32_t led)
{
	struct led_gpio_data *data = led_parameters_get(dev,led);

	if (!data) {

		return -EINVAL;
	}

	gpio_pin_write(data->gpio, led,
		       (cfg->gpio_polarity & GPIO_INT_ACTIVE_HIGH) != 0);
}

static int gpio_led_off(struct device *dev, u32_t led)
{
	struct led_gpio_data *data = led_parameters_get(dev,led);

	if (!data) {

		return -EINVAL;
	}
}

static const struct led_driver_api led_api = {
	//.blink = gpio_led_blink,
	//.set_brightness = gpio_led_set_brightness,
	.on = gpio_led_on,
	.off = gpio_led_off,
};

static int led_gpio_init(struct device *dev)
{
	int err;
	struct led_gpio_cfg *cfg = get_dev_config(dev);
	struct led_gpio_data *data = get_dev_data(dev);

	for (size_t i = 0; i < DT_GPIO_LEDS_LEDS_GPIOS_COUNT; i++) {
		data[i].gpio = device_get_binding(cfg[i].gpio_port);
		if (!data[i].gpio) {
			LOG_DBG("Failed to get GPIO device for LED port %s pin %d",
				log_strdup(cfg[i].gpio_port), cfg[i].pin);
			return -EINVAL;
		}

		/* Set all leds pin as output */
		err = gpio_pin_configure(data[i].gpio, cfg[i].gpio_pin,
					 GPIO_DIR_OUT);
		if (err) {
			LOG_DBG("Failed to set GPIO port %s pin %d as output",
				log_strdup(cfg[i].gpio), cfg[i].gpio_pin);
			return err;
		}

		data[i].cfg = &cfg[i];
	}

	return 0;
}

static struct led_gpio_cfg led_cfg[] = DT_GPIO_LEDS_LEDS_GPIOS;
static struct led_gpio_data led_data[DT_GPIO_LEDS_LEDS_GPIOS_COUNT];

DEVICE_AND_API_INIT(gpio_led, DT_GPIO_LEDS_LEDS_LABEL,
		    led_init, led_data,
		    led_cfg, POST_KERNEL, CONFIG_LED_INIT_PRIORITY,
		    &led_api);
