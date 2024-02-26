#pragma once

#include <source_location>
#include <system_error>

class MajorError
{
  public:
    MajorError() = delete;
    explicit MajorError(std::error_code code, int err_no, const std::source_location &loc = std::source_location::current()) : _code{code}, _err_no{err_no}, _location{loc} {}

    [[nodiscard]] std::error_code code() const {return _code;}
    [[nodiscard]] int err_no() const {return _err_no;}
    [[nodiscard]] std::source_location where()const {return _location;}

  private:
    const std::error_code _code;
    const int _err_no;
    const std::source_location _location;
};