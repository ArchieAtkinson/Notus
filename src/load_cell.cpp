#include "load_cell.hpp"

#include "zephyr/drivers/adc.h"

#include "logging.hpp"
#include <cstddef>
#include <cstdint>


LOG_MODULE_REGISTER(load_cell, LOG_LEVEL_DBG);


LoadCell::LoadCell(const struct adc_dt_spec adc_spec, float grams_per_millivolt) : _adc_spec{adc_spec}, _grams_per_millivolt{grams_per_millivolt}
{
    if (!adc_is_ready_dt(&_adc_spec))
    {
        throw LoadCellError();
    }
    if (adc_channel_setup_dt(&_adc_spec) != 0)
    {
        throw LoadCellError();
    }

    _sequence_options = {
        .interval_us     = 0,
        .callback        = nullptr,
        .user_data       = nullptr,
        .extra_samplings = 0,
    };

    _sequence = {
        .options      = &_sequence_options,
        .channels     = BIT(_adc_spec.channel_id),
        .buffer       = &_buffer,
        .buffer_size  = sizeof(_buffer),
        .resolution   = _adc_spec.resolution,
        .oversampling = _adc_spec.oversampling,
        .calibrate    = false,
    };
}

float LoadCell::get_weight_grams()
{
    int ret = adc_read_dt(&_adc_spec, &_sequence);
    if (ret != 0)
    {
        LOG_ERR("ADC Read Failure: %d", ret);
    }

    auto value = static_cast<int32_t>(_buffer);
    ret = adc_raw_to_millivolts_dt(&_adc_spec, &value);
    if (ret != 0)
    {
        LOG_ERR("ADC Read Failure: %d", ret);
    }

    float grams = static_cast<float>(value) * _grams_per_millivolt;
    return grams;
}