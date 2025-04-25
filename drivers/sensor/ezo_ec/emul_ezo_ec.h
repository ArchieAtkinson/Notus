#ifndef APP_INCLUDE_EMUL_ezo_ec_H_
#define APP_INCLUDE_EMUL_ezo_ec_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "ezo_ec_util.h"
#include <zephyr/drivers/i2c_emul.h>

uint8_t emul_ezo_ec_get_reg(const struct emul *emul, uint8_t address);
void emul_ezo_ec_set_reg(const struct emul *emul, uint8_t address, uint8_t reg_value);
void emul_ezo_ec_reset_registers(const struct emul *emul);

struct ezo_ec_config emul_ezo_ec_get_config(const struct emul *emul);
void emul_ezo_ec_set_config(const struct emul *emul, struct ezo_ec_config config);

#ifdef __cplusplus
}
#endif

#endif // APP_INCLUDE_EMUL_ezo_ec_H_
