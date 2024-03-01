#include <cstdint>

#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/kernel.h>

#include "errors.hpp"
#include "testing.hpp"

#include "load_cell.hpp"

namespace
{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct adc_dt_spec adc_spec = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);
#pragma GCC diagnostic pop

ZTEST(load_cell, test_read)
{
    const float grams_per_millivolt = 1.5F;
    LoadCell load_cell(adc_spec, grams_per_millivolt);

    const uint32_t test_value = 500;
    int ret = adc_emul_const_value_set(adc_spec.dev, adc_spec.channel_id, test_value);
    zassert_equal(ret, 0);

    float weight = load_cell.get_weight_grams();
    zassert_within(test_value * grams_per_millivolt, weight, 1.0F * grams_per_millivolt);
}

ZTEST(load_cell, test_constructor)
{
    auto try_and_catch_constructor = [&](LoadCellError error)
    {
        try
        {
            LoadCell load_cell(adc_spec, 1.0F);
        }
        catch (const MajorError &except)
        {
            zassert_equal(except.code(), error);
            return;
        }
        catch (...)
        {
            zassert_unreachable();
        }
        zassert_unreachable();
    };
    
    adc_spec.dev->state->initialized = false;
    try_and_catch_constructor(LoadCellError::adc_is_ready);
    adc_spec.dev->state->initialized = true;

    adc_spec.channel_cfg_dt_node_exists = false;
    try_and_catch_constructor(LoadCellError::adc_channel_setup);
    adc_spec.channel_cfg_dt_node_exists = true;

    try
    {
        LoadCell load_cell(adc_spec, 1.0F);
    }
    catch (...)
    {
        zassert_unreachable();
    }
  
}

ZTEST_SUITE(load_cell, nullptr, nullptr, nullptr, nullptr, nullptr); // NOLINT

} // namespace