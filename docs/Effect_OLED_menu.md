# Menu Root
- Name: Breathing
  - Index: 0
  - Parameters
    - Speed
      - Millisecond delay between steps (uint8_t)
      - Step size (float)
    - HSV color
- Name: Meter
  - Index: 1
  - Parameters
    - HSV color
    - Meter value (uint8_t)
    - Flip (bool)
    - Mirror (bool)
- Name: Rainbow
  - Index: 2
  - Parameters
    - Delta hue (int8_t)
    - Scroll hue (uint8_t)
    - Millisecond delay between steps (uint8_t)
    - Starting HSV color
- **TODO:** Strobe effect

## Effect template
- (Displayed, but not selectable): Effect name
- Effect index
  - Value
  - (Displayed, but not selectable): Parameters
  - Effect parameter 0
    - Value 0
  - Effect parameter 1
    - Value 1
  - ...
  - Effect parameter n
    - Value n

# Actions
- Encoder rotation click
  - Item selected: Change item value
  - Item not selected: Move through items at same level as current item
- Encoder press: Select or deselect current item
- Back button press: Move back one level in menu hierarchy

# Structs and Member Types
## colorRGB
- red (uint8_t)
- green (uint8_t)
- blue (uint8_t)
## colorHSV
- hue (uint16_t)
  - Range: [0, 360)
- saturation (float)
  - Range: [0.0, 1.0]
- value (float)
  - Range: [0.0, 1.0]