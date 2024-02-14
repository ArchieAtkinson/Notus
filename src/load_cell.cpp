#include "load_cell.hpp"
#include "logging.hpp"
#include "zephyr/device.h"


LOG_MODULE_REGISTER(load_cell, LOG_LEVEL_DBG);


LoadCell::LoadCell(const struct device *adc_dev) : _dev{adc_dev}
{

}