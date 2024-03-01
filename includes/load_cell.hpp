#pragma once

#include <cstdint>
#include <system_error>

#include "zephyr/drivers/adc.h"


enum class LoadCellError
{
    adc_is_ready = 1,
    adc_channel_setup,
    adc_read,
    adc_raw_to_millivolt,
};

namespace std
{
  template <>
    struct is_error_code_enum<LoadCellError> : true_type {};
}

std::error_code make_error_code(LoadCellError);

class ILoadCell
{
  public:
    ILoadCell() = default;
    virtual ~ILoadCell() = default;
    virtual float get_weight_grams() = 0;

    ILoadCell(const ILoadCell &other) = delete;
    ILoadCell(ILoadCell&& other) noexcept = delete;
    ILoadCell& operator=(const ILoadCell &other) = delete;
    ILoadCell& operator=(ILoadCell&& other) noexcept = delete;
};

class LoadCell : public ILoadCell
{
  public:
    LoadCell() = delete;
    explicit LoadCell(const struct adc_dt_spec adc_spec, float grams_per_millivolt);
    float get_weight_grams() final;


  private:
    const struct adc_dt_spec _adc_spec;
    const float              _grams_per_millivolt;
    struct adc_sequence_options _sequence_options {};
    struct adc_sequence         _sequence {};
    uint16_t _buffer {};
};