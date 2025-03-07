#include <cassert>
#include <zephyr/kernel.h>
#include <iostream>

#include "custom_errors.hpp"

// Define your error enums
enum class NetworkError
{
    ConnectionFailed,
    Timeout,
    InvalidAddress
};

enum class FileError
{
    NotFound,
    PermissionDenied,
    DiskFull
};

enum class Servity
{
    Major,
    Minor
};

// Define a concrete domain for NetworkError
class network_error_domain : public error_domain<NetworkError>
{
  public:
    constexpr network_error_domain() noexcept : error_domain<NetworkError>("network")
    {
    }

    constexpr std::string_view get_message(NetworkError code) const noexcept override
    {
        switch (code)
        {
        case NetworkError::ConnectionFailed:
            return "Connection failed";
        case NetworkError::Timeout:
            return "Connection timed out";
        case NetworkError::InvalidAddress:
            return "Invalid address";
        default:
            return "Unknown network error";
        }
    }

    // template<>
    // constexpr bool equivlent(const Servity &lhs, const error_code &rhs)
    // {
    //     auto err = static_cast<NetworkError>(rhs.value());
    //     switch (lhs)
    //     {
    //         case Servity::Major: {
    //             return NetworkError::ConnectionFailed == err ||
    //                     NetworkError::Timeout == err;
    //         }
    //         case Servity::Minor: {
    //                 return NetworkError::InvalidAddress == err;
    //         }
    //     }
    // }
};

const network_error_domain network_domain{};

constexpr error_code make_error_code(NetworkError err)
{
    return {err, network_domain};
}

class file_error_domain : public error_domain<FileError>
{
  public:
    constexpr file_error_domain() noexcept : error_domain<FileError>("file")
    {
    }

    constexpr std::string_view get_message(FileError code) const noexcept override
    {
        switch (code)
        {
        case FileError::NotFound:
            return "File not found";
        case FileError::PermissionDenied:
            return "Permission denied";
        case FileError::DiskFull:
            return "Disk is full";
        default:
            return "Unknown file error";
        }
    }    
};

const file_error_domain file_domain{};

constexpr error_code make_error_code(FileError err)
{
    return {err, file_domain};
}

// Usage
int main()
{
    printk("Here\n");
    
    error_code ec1 = make_error_code(NetworkError::Timeout);
    error_code ec2 = make_error_code(FileError::NotFound);

    std::cout << "Error: " << ec1.message() << " from domain: " << ec1.domain().name() << std::endl;

    

    return 0;
}
