#ifndef MODBUS_TESTER_MB_TESTER_COMMON_H
#define MODBUS_TESTER_MB_TESTER_COMMON_H

#include <vector>
#include <string>

using mb_reg_vector = std::vector<uint16_t>;
using mb_bit_vector = std::vector<uint8_t>;

//
// Modbus exception
//
class mb_exception : public std::exception
{
public:
    mb_exception(std::string m, int err_no);

    const char* what() const noexcept override
    {
        return msg.c_str();
    }

    int error_code() const noexcept
    {
        return err_code;
    }

    const char* error_descr() const noexcept;

    static std::string build_message(std::string const& what, int err_code);

private:
    static const char* error_descr(int code) noexcept;

    std::string msg;
    int err_code;
};

#endif //MODBUS_TESTER_MB_TESTER_COMMON_H