// SNES platform implementation for eLua

#include "platform.h"
#include "platform_conf.h"
#include "type.h"
#include "devman.h"
#include "elua_int.h"

// Basic platform initialization for SNES
int platform_init(void) {
    // Initialize basic platform services
    // For SNES, we don't need complex platform initialization
    return PLATFORM_OK;
}

// Platform interrupt initialization
void platform_int_init(void) {
    // Initialize interrupts for SNES
    // This will be implemented as needed
}

// Basic platform functions that eLua expects
int platform_pio_has_port(unsigned port) {
    return 0; // No PIO support on SNES
}

const char* platform_pio_get_prefix(unsigned port) {
    return NULL;
}

int platform_pio_has_pin(unsigned port, unsigned pin) {
    return 0;
}

u32 platform_pio_op(unsigned port, u32 pinmask, int op) {
    return 0;
}

int platform_timer_exists(unsigned id) {
    return (id == PLATFORM_TIMER_SYS_ID) ? 1 : 0;
}

void platform_timer_delay(unsigned id, timer_data_type delay_us) {
    // Simple delay implementation
    volatile timer_data_type i;
    for (i = 0; i < delay_us; i++) {
        // Simple busy wait
    }
}

timer_data_type platform_timer_op(unsigned id, int op, timer_data_type data) {
    return 0;
}

int platform_timer_set_match_int(unsigned id, timer_data_type period_us, int type) {
    return 0;
}

timer_data_type platform_timer_get_diff_us(unsigned id, timer_data_type start, timer_data_type end) {
    return end - start;
}

timer_data_type platform_timer_read_sys(void) {
    // Return a simple counter
    static timer_data_type counter = 0;
    return counter++;
}

int platform_timer_sys_available(void) {
    return 1;
}

u64 platform_timer_sys_raw_read(void) {
    return (u64)platform_timer_read_sys();
}

void platform_timer_sys_enable_int(void) {
    // Enable system timer interrupt
}

void platform_timer_sys_disable_int(void) {
    // Disable system timer interrupt
}

// Stub implementations for other platform functions
int platform_cpu_set_global_interrupts(int status) { return 0; }
int platform_cpu_get_global_interrupts(void) { return 0; }
int platform_cpu_set_interrupt(elua_int_id id, elua_int_resnum resnum, int status) { return 0; }
int platform_cpu_get_interrupt(elua_int_id id, elua_int_resnum resnum) { return 0; }
int platform_cpu_get_interrupt_flag(elua_int_id id, elua_int_resnum resnum, int clear) { return 0; }
u32 platform_cpu_get_frequency(void) { return 1000000; }

// UART stubs
int platform_uart_exists(unsigned id) { return 0; }
u32 platform_uart_setup(unsigned id, u32 baud, int databits, int parity, int stopbits) { return 0; }
int platform_uart_set_buffer(unsigned id, unsigned size) { return 0; }
void platform_uart_send(unsigned id, u8 data) { }
int platform_uart_recv(unsigned id, unsigned timer_id, timer_data_type timeout) { return -1; }
int platform_uart_set_flow_control(unsigned id, int type) { return 0; }

// Other stubs
int platform_spi_exists(unsigned id) { return 0; }
u32 platform_spi_setup(unsigned id, int mode, u32 clock, unsigned cpol, unsigned cpha, unsigned databits) { return 0; }
u32 platform_spi_send_recv(unsigned id, u32 data) { return 0; }
void platform_spi_select(unsigned id, int is_select) { }

int platform_pwm_exists(unsigned id) { return 0; }
u32 platform_pwm_setup(unsigned id, u32 frequency, unsigned duty) { return 0; }
void platform_pwm_start(unsigned id) { }
void platform_pwm_stop(unsigned id) { }
u32 platform_pwm_set_clock(unsigned id, u32 data) { return 0; }
u32 platform_pwm_get_clock(unsigned id) { return 0; }

int platform_adc_exists(unsigned id) { return 0; }
u32 platform_adc_get_maxval(unsigned id) { return 0; }
u32 platform_adc_set_smoothing(unsigned id, u32 length) { return 0; }
void platform_adc_set_blocking(unsigned id, u32 mode) { }
void platform_adc_set_freerunning(unsigned id, u32 mode) { }
u32 platform_adc_is_done(unsigned id) { return 0; }
void platform_adc_set_timer(unsigned id, u32 timer) { }
int platform_adc_update_sequence(void) { return 0; }
int platform_adc_start_sequence(void) { return 0; }
void platform_adc_stop(unsigned id) { }
u32 platform_adc_set_clock(unsigned id, u32 frequency) { return 0; }
int platform_adc_check_timer_id(unsigned id, unsigned timer_id) { return 0; }

int platform_i2c_exists(unsigned id) { return 0; }
u32 platform_i2c_setup(unsigned id, u32 speed) { return 0; }
void platform_i2c_send_start(unsigned id) { }
void platform_i2c_send_stop(unsigned id) { }
int platform_i2c_send_address(unsigned id, u16 address, int direction) { return 0; }
int platform_i2c_send_byte(unsigned id, u8 data) { return 0; }
int platform_i2c_recv_byte(unsigned id, int ack) { return 0; }

int platform_can_exists(unsigned id) { return 0; }
u32 platform_can_setup(unsigned id, u32 clock) { return 0; }
void platform_can_send(unsigned id, u32 canid, u8 idtype, u8 len, const u8 *data) { }
int platform_can_recv(unsigned id, u32 *canid, u8 *idtype, u8 *len, u8 *data) { return 0; }

void platform_eth_send_packet(const void* src, u32 size) { }
u32 platform_eth_get_packet_nb(void* buf, u32 maxlen) { return 0; }
void platform_eth_force_interrupt(void) { }
u32 platform_eth_get_elapsed_time(void) { return 0; }

void platform_usb_cdc_send(u8 data) { }
int platform_usb_cdc_recv(s32 timeout) { return -1; }

u32 platform_flash_get_first_free_block_address(u32 *psect) { return 0; }
u32 platform_flash_get_sector_of_address(u32 addr) { return 0; }
u32 platform_flash_write(const void *from, u32 toaddr, u32 size) { return 0; }
u32 platform_flash_get_num_sectors(void) { return 0; }
int platform_flash_erase_sector(u32 sector_id) { return 0; }

void* platform_get_first_free_ram(unsigned id) { return NULL; }
void* platform_get_last_free_ram(unsigned id) { return NULL; }
