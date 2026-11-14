#include "fake_gpio.h"

static void fake_write(void *ctx, uint8_t pin, gpio_level_t level)
{
    fake_gpio_t *fake = (fake_gpio_t *)ctx;
    if (fake == NULL) {
        return;
    }
    if (fake->log_count < FAKE_GPIO_LOG_SIZE) {
        fake->log[fake->log_count].pin = pin;
        fake->log[fake->log_count].level = level;
        fake->log_count++;
    }
    fake->total_writes++;
}

static gpio_level_t fake_read(void *ctx, uint8_t pin)
{
    fake_gpio_t *fake = (fake_gpio_t *)ctx;
    if (fake == NULL) {
        return GPIO_LOW;
    }
    fake->reads++;
    if (pin >= FAKE_GPIO_PINS) {
        return GPIO_LOW;
    }
    return fake->input_level[pin];
}

void fake_gpio_init(fake_gpio_t *fake, hal_gpio_t *hal)
{
    if (fake == NULL || hal == NULL) {
        return;
    }
    for (size_t i = 0u; i < FAKE_GPIO_LOG_SIZE; i++) {
        fake->log[i].pin = 0u;
        fake->log[i].level = GPIO_LOW;
    }
    fake->log_count = 0u;
    fake->total_writes = 0u;
    for (size_t i = 0u; i < FAKE_GPIO_PINS; i++) {
        fake->input_level[i] = GPIO_LOW;
    }
    fake->reads = 0u;
    hal->write = fake_write;
    hal->read = fake_read;
    hal->ctx = fake;
}