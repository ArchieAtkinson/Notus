#pragma once

#include <chrono>


class IDisplay
{
  public:
    IDisplay() = default;
    virtual ~IDisplay() = default;

    virtual void set_weight_grams(double weight_grams) = 0;
    virtual void set_time(std::chrono::milliseconds milliseconds) = 0;    

    IDisplay(const IDisplay &other) = delete;
    IDisplay(IDisplay&& other) noexcept = delete;
    IDisplay& operator=(const IDisplay &other) = delete;
    IDisplay& operator=(IDisplay&& other) noexcept = delete;
    
};

class Display final : public IDisplay
{
  public:
    void set_weight_grams(double weight_grams) final;
    void set_time(std::chrono::milliseconds milliseconds) final;    
};