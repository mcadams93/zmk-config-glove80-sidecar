/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_absolute_to_relative

#include <drivers/input_processor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(absolute_to_relative, CONFIG_ZMK_LOG_LEVEL);

/* Maximum plausible single-sample delta in absolute coordinates.
 * Anything larger is a spurious jump (lift-off, first-touch residual, glitch). */
#define MAX_DELTA 100

/* --- Acceleration curve (Apple-style non-linear) ---
 * Slow finger movements get low gain for precision;
 * fast movements get high gain for covering distance.
 *
 * We use integer-only piecewise linear interpolation.
 * Speed thresholds are in absolute coordinate units per sample.
 *
 * |delta| <= ACCEL_SLOW:  multiply by ACCEL_SLOW_NUM / ACCEL_SLOW_DEN  (0.6x)
 * |delta| >= ACCEL_FAST:  multiply by ACCEL_FAST_NUM / ACCEL_FAST_DEN  (2.5x)
 * In between: linear interpolation between the two.
 */
#define ACCEL_SLOW      3
#define ACCEL_FAST      15
#define ACCEL_SLOW_NUM  3
#define ACCEL_SLOW_DEN  5   /* 3/5 = 0.6x for micro-movements */
#define ACCEL_FAST_NUM  5
#define ACCEL_FAST_DEN  2   /* 5/2 = 2.5x for fast swipes */

static inline int16_t apply_acceleration(int16_t delta) {
    int16_t sign = (delta >= 0) ? 1 : -1;
    int16_t mag = delta * sign; /* abs(delta) */

    int32_t result;
    if (mag <= ACCEL_SLOW) {
        result = (int32_t)mag * ACCEL_SLOW_NUM / ACCEL_SLOW_DEN;
    } else if (mag >= ACCEL_FAST) {
        result = (int32_t)mag * ACCEL_FAST_NUM / ACCEL_FAST_DEN;
    } else {
        /* Linear interpolation between slow and fast gain.
         * factor = slow_factor + (fast_factor - slow_factor) * (mag - ACCEL_SLOW)
         *                                                     / (ACCEL_FAST - ACCEL_SLOW)
         * To stay integer: compute numerator scaled by a common denominator.
         *   slow_factor = ACCEL_SLOW_NUM / ACCEL_SLOW_DEN
         *   fast_factor = ACCEL_FAST_NUM / ACCEL_FAST_DEN
         * Common denom = ACCEL_SLOW_DEN * ACCEL_FAST_DEN = 10
         * slow_scaled = ACCEL_SLOW_NUM * ACCEL_FAST_DEN = 6
         * fast_scaled = ACCEL_FAST_NUM * ACCEL_SLOW_DEN = 25
         */
        int32_t slow_scaled = (int32_t)ACCEL_SLOW_NUM * ACCEL_FAST_DEN; /* 6 */
        int32_t fast_scaled = (int32_t)ACCEL_FAST_NUM * ACCEL_SLOW_DEN; /* 25 */
        int32_t range = ACCEL_FAST - ACCEL_SLOW; /* 12 */
        int32_t pos = mag - ACCEL_SLOW;
        int32_t factor_scaled = slow_scaled + (fast_scaled - slow_scaled) * pos / range;
        int32_t common_den = (int32_t)ACCEL_SLOW_DEN * ACCEL_FAST_DEN; /* 10 */
        result = (int32_t)mag * factor_scaled / common_den;
    }

    /* Ensure at least 1 unit of movement if there was any input */
    if (result == 0 && mag > 0) {
        result = 1;
    }

    return (int16_t)(result * sign);
}

struct absolute_to_relative_config {
    uint8_t time_between_normal_reports;
};

struct absolute_to_relative_data {
    uint16_t previous_x, previous_y;
    bool touching_x, touching_y;
    uint8_t samples_x, samples_y;
    const struct device *dev;
    struct k_work_delayable touch_end_timeout_work;
};
 
static int absolute_to_relative_handle_event(const struct device *dev, struct input_event *event, uint32_t param1,
                               uint32_t param2, struct zmk_input_processor_state *state) { 
    const struct absolute_to_relative_config *config = dev->config;
    struct absolute_to_relative_data *data = (struct absolute_to_relative_data *)dev->data;

    k_work_reschedule(&data->touch_end_timeout_work, K_MSEC(config->time_between_normal_reports));

    if (event->type == INPUT_EV_ABS) {
        uint16_t value = event->value;

        if (event->code == INPUT_ABS_X) {
            if (!data->touching_x) {
                data->touching_x = true;
                data->samples_x = 0;
            }
            data->samples_x++;
            if (data->samples_x <= 2) {
                /* Suppress the first two samples after finger-down to
                 * prevent cursor jump.  The first sample has no prior
                 * reference and the second often carries a noisy delta. */
                data->previous_x = value;
                event->type = 0;
                event->code = 0;
                event->value = 0;
            } else {
                int16_t delta = (int16_t)(value - data->previous_x);
                data->previous_x = value;
                if (delta > MAX_DELTA || delta < -MAX_DELTA) {
                    event->type = 0;
                    event->code = 0;
                    event->value = 0;
                } else {
                    event->type = INPUT_EV_REL;
                    event->code = INPUT_REL_X;
                    event->value = apply_acceleration(delta);
                }
            }
        } else if (event->code == INPUT_ABS_Y) {
            if (!data->touching_y) {
                data->touching_y = true;
                data->samples_y = 0;
            }
            data->samples_y++;
            if (data->samples_y <= 2) {
                data->previous_y = value;
                event->type = 0;
                event->code = 0;
                event->value = 0;
            } else {
                int16_t delta = (int16_t)(value - data->previous_y);
                data->previous_y = value;
                if (delta > MAX_DELTA || delta < -MAX_DELTA) {
                    event->type = 0;
                    event->code = 0;
                    event->value = 0;
                } else {
                    event->type = INPUT_EV_REL;
                    event->code = INPUT_REL_Y;
                    event->value = apply_acceleration(delta);
                }
            }
        }
    }

    return ZMK_INPUT_PROC_CONTINUE;
}

static void touch_end_timeout_callback(struct k_work *work) {
    const struct device *dev = DEVICE_DT_INST_GET(0);
    struct absolute_to_relative_data *data = (struct absolute_to_relative_data *)dev->data;
    data->touching_x = false;
    data->touching_y = false;
}

static int absolute_to_relative_init(const struct device *dev) {
    struct absolute_to_relative_data *data = (struct absolute_to_relative_data *)dev->data;
    k_work_init_delayable(&data->touch_end_timeout_work, touch_end_timeout_callback);

    return 0;
}

static const struct zmk_input_processor_driver_api absolute_to_relative_driver_api = {
    .handle_event = absolute_to_relative_handle_event,
};

#define ABSOLUTE_TO_RELATIVE_INST(n)                                                                    \
    static struct absolute_to_relative_data processor_absolute_to_relative_data_##n = {                 \
        .touching_x = false,                                                                              \
        .touching_y = false,                                                                              \
        .samples_x = 0,                                                                                   \
        .samples_y = 0,                                                                                   \
    };                                                                                                  \
    static const struct absolute_to_relative_config processor_absolute_to_relative_config_##n = {       \
        .time_between_normal_reports = DT_INST_PROP_OR(n, time_between_normal_reports, 30),             \
    };                                                                                                  \
    DEVICE_DT_INST_DEFINE(n, absolute_to_relative_init, NULL, &processor_absolute_to_relative_data_##n, \
                          &processor_absolute_to_relative_config_##n, POST_KERNEL,                      \
                          CONFIG_INPUT_PROCESSOR_ABSOLUTE_TO_RELATIVE_INIT_PRIORITY, &absolute_to_relative_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ABSOLUTE_TO_RELATIVE_INST)
