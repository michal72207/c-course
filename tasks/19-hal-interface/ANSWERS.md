# Task 19 — Understanding questions

Answer in your own words, a few sentences each. Write under each `Answer:` line.

## Q1. Your `led.c` never includes a hardware header or touches a register. What would you need to write to run it on a real board, and what in `led.c` would change?

Answer:
To run on a real board, I would only need to write a hardware-specific `hal_gpio_t` implementation whose `write` and `read` functions access the microcontroller's actual GPIO registers, and pass that `hal_gpio_t` to `led_init`. Nothing inside `led.c` would need to change at all, because it depends solely on the abstract HAL interface.

## Q2. What is `ctx` for? Why can't `fake_write` just use a global `fake_gpio_t`?

Answer:
`ctx` is a generic context pointer that passes the specific `fake_gpio_t` instance state into the callback functions. Using a global variable instead of `ctx` would restrict the program to a single global fake instance, preventing multiple independent GPIO mocks from existing simultaneously or running isolated tests cleanly.

## Q3. Why are the teacher's mock and your fake both useful? What does testing the LED driver on a mock give you that testing on the real board doesn't?

Answer:
Testing on a mock/fake allows instant, automated verification on a PC without flashing hardware, lets us record and assert the exact sequence and count of pin writes (such as ensuring no extra glitches occur), and makes it trivial to simulate edge cases or button states deterministically.

## Q4. What does "active low" mean electrically, and why does the driver handle it instead of the code that calls `led_set`?

Answer:
Electrically, "active low" means the circuit is connected to VCC so driving the pin to 0 V (`GPIO_LOW`) completes the circuit and turns the LED on (or pulling a button pin to 0 V indicates a press). Handling polarity inside the driver hides hardware wiring details from application logic, allowing higher-level code to use clean boolean states (`on`/`pressed`) regardless of board wiring.