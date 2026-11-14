#include "uart_rx.h"

#include "sim_uart.h"

static volatile uint8_t rx_buf[UART_RX_BUF_SIZE];
static volatile size_t rx_head = 0u;
static volatile size_t rx_tail = 0u;
static volatile size_t rx_count = 0u;
static volatile size_t rx_lines = 0u;
static volatile uint32_t rx_dropped = 0u;

void uart_rx_init(void)
{
    hal_irq_disable();
    rx_head = 0u;
    rx_tail = 0u;
    rx_count = 0u;
    rx_lines = 0u;
    rx_dropped = 0u;
    hal_uart_attach_isr(uart_rx_isr);
    hal_irq_enable();
}

void uart_rx_isr(void)
{
    uint8_t byte = hal_uart_read_dr();
    if (rx_count >= UART_RX_BUF_SIZE) {
        rx_dropped++;
        return;
    }
    rx_buf[rx_head] = byte;
    rx_head = (rx_head + 1u) % UART_RX_BUF_SIZE;
    rx_count++;
    if (byte == (uint8_t)'\n') {
        rx_lines++;
    }
}

bool uart_rx_line_ready(void)
{
    return rx_lines > 0u;
}

bool uart_rx_read_line(char *dst, size_t dst_size)
{
    if (dst == NULL || dst_size == 0u) {
        return false;
    }
    hal_irq_disable();
    if (rx_lines == 0u) {
        hal_irq_enable();
        return false;
    }
    rx_lines--;
    hal_irq_enable();

    size_t out_idx = 0u;
    while (true) {
        hal_irq_disable();
        if (rx_count == 0u) {
            hal_irq_enable();
            break;
        }
        uint8_t byte = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1u) % UART_RX_BUF_SIZE;
        rx_count--;
        hal_irq_enable();

        if (byte == (uint8_t)'\n') {
            break;
        }
        if (out_idx + 1u < dst_size) {
            dst[out_idx++] = (char)byte;
        }
    }
    if (out_idx > 0u && dst[out_idx - 1u] == '\r') {
        out_idx--;
    }
    dst[out_idx] = '\0';
    return true;
}

size_t uart_rx_available(void)
{
    hal_irq_disable();
    size_t avail = rx_count;
    hal_irq_enable();
    return avail;
}

uint32_t uart_rx_dropped(void)
{
    hal_irq_disable();
    uint32_t dropped = rx_dropped;
    hal_irq_enable();
    return dropped;
}