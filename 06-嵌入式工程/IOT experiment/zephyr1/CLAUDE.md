# CLAUDE.md

## Project Goal

Bring up Zephyr RTOS on a custom STM32G070CBT6 board using a native Zephyr application and a custom board port.

This project must not try to embed Zephyr into the STM32CubeMX generated bare-metal project. The existing CubeMX files are hardware references only.

## Target Hardware

- MCU: STM32G070CBT6
- Package: LQFP48
- Core: Arm Cortex-M0+
- Flash: 128 KB
- SRAM: 36 KB
- Zephyr SoC include: `st/g0/stm32g070Xb.dtsi`
- Zephyr pinctrl include: `st/g0/stm32g070cbtx-pinctrl.dtsi`

## Board Pin Map

### System And Clock

- `VBAT`: backup domain power
- `VDD/VDA`: 3V3
- `VSS`: GND
- `NRST`: reset
- `PC14`: LSE OSC32_IN
- `PC15`: LSE OSC32_OUT
- `PA13`: SWDIO
- `PA14`: SWCLK / BOOT0

### UART

- `USART4_TX`: PA0
- `USART4_RX`: PA1
- `USART2_TX`: PA2
- `USART2_RX`: PA3
- `USART1_TX`: PA9
- `USART1_RX`: PA10
- `USART3_TX`: PB10
- `USART3_RX`: PB11

Initial console should use `USART2` on `PA2/PA3` at `115200`.

### GPIO

- `LED1`: PA11
- `LED2`: PA12
- `SW1`: PD0
- `SW2`: PD1
- `SW3`: PD2
- `SW4`: PD3

Assume LEDs are active high and buttons are active low until verified against the schematic or hardware.

### LoRa Related Pins

Likely SPI1 mapping:

- `SPI1_NSS/CS`: PA4
- `SPI1_SCK`: PA5
- `SPI1_MISO`: PA6
- `SPI1_MOSI`: PA7

Additional LoRa GPIO candidates:

- PB0
- PB1
- PB2
- PB13
- PB14

Do not bind a specific LoRa driver until the exact U6 module or radio chip is known.

### Other Pins

- `PA8`: connected to R4/R5 network
- `PB12`: connected to adjustable resistor R8 network
- `PB8/PB9`: connected to digital microphone U4 and pull-ups
- `PC13`: GPIO
- `PC6`: GPIO
- `PC7`: GPIO

Confirm the digital microphone interface before enabling I2C, I2S, or PDM on PB8/PB9.

## Recommended Repository Layout

Create a Zephyr workspace outside paths containing spaces, for example:

```text
D:/nanyun/zephyrproject/
  zephyr/
  modules/
  applications/
    zephyr1/
      CMakeLists.txt
      prj.conf
      src/
        main.c
      boards/
        others/
          zephyr1/
            board.yml
            board.cmake
            zephyr1.dts
            zephyr1_defconfig
```

The current CubeMX project directory may keep this `CLAUDE.md` and the generated CubeMX project as reference material, but the Zephyr application should be native Zephyr.

## Non-Negotiable Rules

1. Do not compile CubeMX startup code into the Zephyr app.
2. Do not use the CubeMX linker script in the Zephyr app.
3. Do not call `HAL_Init()`, `SystemClock_Config()`, or `MX_GPIO_Init()` from Zephyr application code.
4. Hardware description belongs in Devicetree.
5. Software feature selection belongs in Kconfig files such as `prj.conf` and board defconfig.
6. Application code must use Zephyr APIs first: GPIO, UART, SPI, I2C, ADC, logging, kernel timing, and threads.
7. Bring up one hardware block at a time and verify after each step.

## Environment Setup Plan

Install required tools:

- Git
- Python
- CMake
- Ninja
- Zephyr SDK
- STM32CubeProgrammer or OpenOCD
- ST-LINK driver if needed

Initialize workspace:

```powershell
py -m pip install west
west init D:\nanyun\zephyrproject
cd D:\nanyun\zephyrproject
west update
west zephyr-export
py -m pip install -r zephyr\scripts\requirements.txt
```

Verify the toolchain with the closest upstream board:

```powershell
west build -b nucleo_g070rb zephyr\samples\hello_world
```

## Custom Board Port Plan

### Step 1: Create Application Skeleton

Create:

```text
applications/zephyr1/CMakeLists.txt
applications/zephyr1/prj.conf
applications/zephyr1/src/main.c
```

Minimal `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr1)

target_sources(app PRIVATE src/main.c)
```

Initial `prj.conf`:

```conf
CONFIG_GPIO=y
CONFIG_SERIAL=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_PRINTK=y
```

### Step 2: Create Board Directory

Create:

```text
applications/zephyr1/boards/others/zephyr1/
```

Add:

```text
board.yml
board.cmake
zephyr1.dts
zephyr1_defconfig
```

Initial `board.yml`:

```yaml
board:
  name: zephyr1
  full_name: STM32G070CBT6 Custom Board
  vendor: others
  socs:
    - name: stm32g070xx
```

Initial `board.cmake`:

```cmake
board_runner_args(stm32cubeprogrammer "--port=swd")
include(${ZEPHYR_BASE}/boards/common/stm32cubeprogrammer.board.cmake)
```

If STM32CubeProgrammer is not available, use OpenOCD or pyOCD later.

### Step 3: Define Board Devicetree

`zephyr1.dts` should start with:

```dts
/dts-v1/;
#include <st/g0/stm32g070Xb.dtsi>
#include <st/g0/stm32g070cbtx-pinctrl.dtsi>
#include <zephyr/dt-bindings/input/input-event-codes.h>

/ {
    model = "STM32G070CBT6 custom board";
    compatible = "custom,zephyr1";

    chosen {
        zephyr,console = &usart2;
        zephyr,shell-uart = &usart2;
        zephyr,sram = &sram0;
        zephyr,flash = &flash0;
    };

    leds {
        compatible = "gpio-leds";

        led1: led_1 {
            gpios = <&gpioa 11 GPIO_ACTIVE_HIGH>;
            label = "LED1";
        };

        led2: led_2 {
            gpios = <&gpioa 12 GPIO_ACTIVE_HIGH>;
            label = "LED2";
        };
    };

    gpio_keys {
        compatible = "gpio-keys";

        sw1: button_1 {
            gpios = <&gpiod 0 GPIO_ACTIVE_LOW>;
            zephyr,code = <INPUT_KEY_0>;
            label = "SW1";
        };

        sw2: button_2 {
            gpios = <&gpiod 1 GPIO_ACTIVE_LOW>;
            zephyr,code = <INPUT_KEY_1>;
            label = "SW2";
        };

        sw3: button_3 {
            gpios = <&gpiod 2 GPIO_ACTIVE_LOW>;
            zephyr,code = <INPUT_KEY_2>;
            label = "SW3";
        };

        sw4: button_4 {
            gpios = <&gpiod 3 GPIO_ACTIVE_LOW>;
            zephyr,code = <INPUT_KEY_3>;
            label = "SW4";
        };
    };

    aliases {
        led0 = &led1;
        led1 = &led2;
        sw0 = &sw1;
        sw1 = &sw2;
        sw2 = &sw3;
        sw3 = &sw4;
    };
};
```

Clock configuration:

```dts
&clk_hsi {
    status = "okay";
};

&pll {
    div-m = <1>;
    mul-n = <8>;
    div-p = <2>;
    div-r = <2>;
    clocks = <&clk_hsi>;
    status = "okay";
};

&rcc {
    clocks = <&pll>;
    clock-frequency = <DT_FREQ_M(64)>;
    ahb-prescaler = <1>;
    apb1-prescaler = <1>;
};
```

USART2 console:

```dts
&usart2 {
    pinctrl-0 = <&usart2_tx_pa2 &usart2_rx_pa3>;
    pinctrl-names = "default";
    current-speed = <115200>;
    status = "okay";
};
```

Optional UARTs:

```dts
&usart1 {
    pinctrl-0 = <&usart1_tx_pa9 &usart1_rx_pa10>;
    pinctrl-names = "default";
    current-speed = <115200>;
    status = "okay";
};

&usart3 {
    pinctrl-0 = <&usart3_tx_pb10 &usart3_rx_pb11>;
    pinctrl-names = "default";
    current-speed = <115200>;
    status = "okay";
};

&usart4 {
    pinctrl-0 = <&usart4_tx_pa0 &usart4_rx_pa1>;
    pinctrl-names = "default";
    current-speed = <115200>;
    status = "okay";
};
```

SPI1 for LoRa transport validation:

```dts
&spi1 {
    pinctrl-0 = <&spi1_sck_pa5 &spi1_miso_pa6 &spi1_mosi_pa7>;
    pinctrl-names = "default";
    cs-gpios = <&gpioa 4 GPIO_ACTIVE_LOW>;
    status = "okay";
};
```

Do not add the final LoRa child node until the exact LoRa chip is known.

### Step 4: Define Board Defconfig

Initial `zephyr1_defconfig`:

```conf
CONFIG_SOC_SERIES_STM32G0X=y
CONFIG_SOC_STM32G070XX=y
CONFIG_SERIAL=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_GPIO=y
```

Keep the initial defconfig small. Add SPI, I2C, ADC, logging, shell, and sensor options only when each feature is brought up.

## Initial Application Plan

`src/main.c` should initially:

1. Print a boot banner.
2. Configure `led0`.
3. Toggle `led0` every 500 ms.
4. Optionally read button states after GPIO output is verified.

Use Zephyr APIs:

- `#include <zephyr/kernel.h>`
- `#include <zephyr/device.h>`
- `#include <zephyr/drivers/gpio.h>`
- `printk()`
- `GPIO_DT_SPEC_GET()`
- `gpio_is_ready_dt()`
- `gpio_pin_configure_dt()`
- `gpio_pin_toggle_dt()`
- `k_sleep()`

## Build And Flash Commands

Build from the Zephyr workspace root:

```powershell
cd D:\nanyun\zephyrproject
west build -b zephyr1 applications\zephyr1
```

Clean rebuild:

```powershell
west build -p always -b zephyr1 applications\zephyr1
```

Flash:

```powershell
west flash
```

Inspect generated Devicetree:

```powershell
Get-Content build\zephyr\zephyr.dts
```

Inspect generated Kconfig:

```powershell
Get-Content build\zephyr\.config
```

## Bring-Up Phases

### Phase 1: Toolchain Validation

Definition of done:

- `nucleo_g070rb` sample builds successfully.
- Zephyr SDK is detected.
- `west` works from the workspace root.

### Phase 2: Custom Board Compiles

Definition of done:

- `west build -b zephyr1 applications\zephyr1` configures successfully.
- `build/zephyr/zephyr.dts` contains `stm32g070Xb`.
- Flash size is 128 KB and SRAM size is 36 KB.

### Phase 3: Console Works

Definition of done:

- USART2 outputs the boot banner at 115200 baud.
- `printk()` output is visible.

### Phase 4: LED Works

Definition of done:

- `led0` toggles on PA11.
- If LED polarity is inverted, update `GPIO_ACTIVE_HIGH` to `GPIO_ACTIVE_LOW`.

### Phase 5: Buttons Work

Definition of done:

- PD0 to PD3 are detected as buttons.
- If button polarity is inverted, update `GPIO_ACTIVE_LOW` to `GPIO_ACTIVE_HIGH`.

### Phase 6: Extra UARTs Work

Definition of done:

- USART1 on PA9/PA10 works.
- USART3 on PB10/PB11 works.
- USART4 on PA0/PA1 works.

### Phase 7: SPI1 Works

Definition of done:

- SPI1 is present in `zephyr.dts`.
- Application can perform an SPI transaction.
- PA4 behaves as chip select.

### Phase 8: LoRa Driver Decision

Before implementation, identify U6:

- SX1276/SX1278 style SPI radio
- SX1262/SX1268 style SPI radio
- UART AT-command LoRa module
- Other module

Only after identification should a driver binding or application protocol be added.

### Phase 9: Low-Speed Clock And RTC

Enable LSE on PC14/PC15 only after basic firmware is stable.

Definition of done:

- LSE starts reliably.
- RTC clock source is correct.
- No startup hang occurs if crystal is absent or incorrectly loaded.

### Phase 10: Application Migration

Move business logic into Zephyr application modules:

```text
src/
  main.c
  board_status.c
  lora_transport.c
  app_protocol.c
```

Keep hardware-specific pin names in Devicetree, not hard-coded in C.

## Debugging Checklist

When build fails:

1. Check board name and path.
2. Check `board.yml` syntax.
3. Check missing Kconfig symbols.
4. Check DTS include paths.
5. Check pinctrl label names.
6. Check generated `build/zephyr/zephyr.dts`.
7. Check generated `build/zephyr/.config`.

When flash fails:

1. Confirm ST-LINK connection.
2. Confirm target power.
3. Confirm SWDIO/SWCLK are not repurposed.
4. Try STM32CubeProgrammer manually.
5. Try mass erase only if preserving flash contents is not required.

When firmware does not boot:

1. Confirm reset pin.
2. Confirm power rails.
3. Confirm clock configuration.
4. Temporarily remove LSE/RTC configuration.
5. Use debugger and break at `main`.
6. Verify flash origin and SRAM size in generated build files.

## Coding Standards

- Prefer Zephyr APIs over STM32 HAL calls.
- Keep board-specific hardware in DTS.
- Keep feature flags in Kconfig.
- Keep application behavior in C files under `src/`.
- Avoid global pin number macros in C.
- Keep each bring-up commit focused on one peripheral.
- Verify each peripheral independently before combining features.

## Anti-Patterns

Do not:

- Mix CubeMX startup files with Zephyr startup files.
- Mix CubeMX linker scripts with Zephyr linker scripts.
- Copy the entire `Drivers/` folder into the Zephyr app.
- Recreate HAL clock initialization in `main.c`.
- Enable all peripherals at once.
- Add LoRa DTS bindings before the module/chip is identified.
- Treat PB8/PB9 as I2C before confirming the digital microphone wiring.

## Useful References

- Zephyr application development:
  `https://docs.zephyrproject.org/latest/develop/application/index.html`
- Zephyr board porting guide:
  `https://docs.zephyrproject.org/latest/hardware/porting/board_porting.html`
- Zephyr STM32G070 reference board:
  `https://docs.zephyrproject.org/latest/boards/st/nucleo_g070rb/doc/index.html`
- Upstream reference board:
  `zephyr/boards/st/nucleo_g070rb/`
- HAL STM32 pinctrl reference:
  `modules/hal/stm32/dts/st/g0/stm32g070cbtx-pinctrl.dtsi`

