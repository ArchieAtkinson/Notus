#include "zephyr/drivers/adc.h"

#include "logging.hpp"
#include "errors.hpp"

#include "load_cell.hpp"

LOG_MODULE_REGISTER(load_cell, LOG_LEVEL_DBG);


LoadCell::LoadCell(const struct adc_dt_spec adc_spec, float grams_per_millivolt) : _adc_spec{adc_spec}, _grams_per_millivolt{grams_per_millivolt}
{
    int ret = static_cast<int>(adc_is_ready_dt(&_adc_spec));
    if (ret == 0)
    {
        throw MajorError(LoadCellError::adc_is_ready, ret);
    }

    ret = adc_channel_setup_dt(&_adc_spec);
    if (ret != 0)
    {
        throw MajorError(LoadCellError::adc_channel_setup, ret);
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
        throw MajorError(LoadCellError::adc_read, ret);
    }

    auto value = static_cast<int32_t>(_buffer);
    ret = adc_raw_to_millivolts_dt(&_adc_spec, &value);
    if (ret != 0)
    {
        throw MajorError(LoadCellError::adc_raw_to_millivolt, ret);
    }

    float grams = static_cast<float>(value) * _grams_per_millivolt;
    return grams;
}