#pragma once

#include "zephyr/device.h"

class ILoadCell
{
  public:
    ILoadCell() = default;
    virtual ~ILoadCell() = default;

    ILoadCell(const ILoadCell &other) = delete;
    ILoadCell(ILoadCell&& other) noexcept = delete;
    ILoadCell& operator=(const ILoadCell &other) = delete;
    ILoadCell& operator=(ILoadCell&& other) noexcept = delete;
};


class LoadCell : public ILoadCell
{
  public:
    LoadCell() = delete;
    explicit LoadCell(const struct device *adc_dev);

  private:
    const struct device *_dev = nullptr;
};