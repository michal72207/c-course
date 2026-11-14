# Task 21 — Understanding questions

Answer in your own words, a few sentences each. Write under each `Answer:` line.

## Q1. What can go wrong if the "lines ready" counter is not `volatile`? Describe what the optimiser is allowed to do with `while (!uart_rx_line_ready()) { }` in the main loop.

Answer:
If the counter is not `volatile`, the compiler sees that nothing inside the main loop modifies it and is allowed to hoist the read out of the loop—caching its initial value (`0`) in a CPU register and turning `while (!uart_rx_line_ready()) { }` into an infinite loop (`while (true)`). As a result, the main loop will never notice when the ISR increments the counter in memory.

## Q2. Your main loop does `count--` and the ISR does `count++`. Explain step by step how an interrupt at the wrong moment loses an update if there is no critical section.

Answer:
Suppose `count` starts at 5. First, the main loop loads `5` from memory into a CPU register to perform `count--`. Before the main loop writes `4` back, the ISR fires, reads `5` from memory, increments it, and writes `6` back to memory. When the main loop resumes, it finishes its interrupted instruction using its stale register value (`5 - 1 = 4`) and overwrites memory with `4`, completely losing the ISR's increment.

## Q3. Why must the critical section be short, and why must the ISR itself be short? What would happen to incoming bytes otherwise?

Answer:
While inside an ISR or a critical section (where interrupts are disabled), the microcontroller cannot respond to new incoming UART interrupts. Because UART hardware typically buffers only one or two bytes, staying in an ISR or critical section too long will cause newly arriving bytes to overwrite unread ones in the hardware register (a hardware overrun), permanently losing incoming data.

## Q4. Why does the ISR read the data register even when the buffer is full and the byte will be dropped?

Answer:
On real UART hardware, the receive interrupt flag remains asserted until software physically reads the data register (`DR`). If the ISR returned without reading `hal_uart_read_dr()`, the hardware interrupt would remain active and immediately re-trigger the ISR in an infinite loop, locking up the processor.