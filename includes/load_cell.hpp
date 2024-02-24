#pragma once

#include <cstdint>
#include <exception>

#include "zephyr/drivers/adc.h"

#include "logging.hpp"


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


class LoadCellError : public std::exception
{
    public:
        LoadCellError()
        {
            LOG_MODULE_DECLARE(load_cell);
            LOG_ERR("%s", error_message);
        }
        [[nodiscard]] const char * what () const noexcept final
        {
            return error_message;
        }

    private:
        const char * error_message = "Load Cell Error";
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