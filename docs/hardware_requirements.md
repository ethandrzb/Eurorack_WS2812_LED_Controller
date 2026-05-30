# Hardware Requirements

## Front Panel 
### UI
- OLED screen
- Encoder
- 6 push buttons
  - Color palette
  - FX change
  - Global settings
  - Mod matrix
  - TODO: MIDI config
  - TODO: Free button connected to gate input 1

### I/O
- 4 CV inputs with passthrough (buffered?)
- 4 gate inputs
- 3 terminals for LED strip (screw or spring?)
- TRS or 5 pin MIDI jacks?

### Rear Panel
- Keyed Eurorack power header
- MIDI input (and thru?)
- 3 terminals for LED strip (same data signal as front panel)
- Header for CV input expander (4 more CV inputs, same spec)
  - I2C ADC
  - DIP switch for I2C address?
  - Put analog front end for expander inputs on expander to save BOM cost

## PCB
- STM32 F446RE MCU and required circuitry
- Supporting circuitry for
  - CV inputs (and buffers if applicable)
    - +- 5 or 10 Vpp input range
    - Should handle 10 Vpp with no damage
  - Gate inputs
    - Should handle up to 10 Vpp pulse with no damage
  - MIDI input
- SWD programming header (either 6-pin or MIPI10)
- USART2 output?