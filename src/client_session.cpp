#include "client_session.h"

client_session::~client_session()
{
}

void client_session::connect()
{
    int rc = modbus_connect(ctx);
    if (rc != 0) {
        on_error("Modbus connect failed");
    }
}

void client_session::set_slave_address(int addr)
{
    int rc = modbus_set_slave(ctx, addr);
    if (rc != 0) {
        on_error("Modbus set slave failed");
    }
}

void client_session::set_response_timeout(unsigned milliseconds)
{
    unsigned sec = milliseconds / 1000;
    unsigned msec = milliseconds - sec * 1000;

    int rc = modbus_set_response_timeout(ctx, sec, msec);
    if (rc != 0) {
        on_error("Modbus set timeout failed");
    }
}

mb_reg_vector client_session::read_holding_registers(int addr, int size)
{
    mb_reg_vector data(size);
    int rc = modbus_read_registers(ctx, addr, size, &data[0]);
    if (rc < 0) {
        on_error("Modbus read holding registers failed");
    }
    return data;
}

mb_reg_vector client_session::read_input_registers(int addr, int size)
{
    mb_reg_vector data(size);
    int rc = modbus_read_input_registers(ctx, addr, size, &data[0]);
    if (rc < 0) {
        on_error("Modbus read input registers failed");
    }
    return data;
}

mb_bit_vector client_session::read_coils(int addr, int size)
{
    mb_bit_vector data(size);
    int rc = modbus_read_bits(ctx, addr, size, &data[0]);
    if (rc < 0) {
        on_error("Modbus read coils failed");
    }
    return data;
}

mb_bit_vector client_session::read_discrete_inputs(int addr, int size)
{
    mb_bit_vector data(size);
    int rc = modbus_read_input_bits(ctx, addr, size, &data[0]);
    if (rc < 0) {
        on_error("Modbus read input bits failed");
    }
    return data;
}

void client_session::write_holding_registers(int addr, mb_reg_vector const& data)
{
    int rc = modbus_write_registers(ctx, addr, data.size(), data.data());
    if (rc < 0) {
        on_error("Modbus write registers failed");
    }
}

void client_session::write_coils(int addr, mb_bit_vector const& data)
{
    int rc = modbus_write_bits(ctx, addr, data.size(), data.data());
    if (rc < 0) {
        on_error("Modbus write coils failed");
    }
}