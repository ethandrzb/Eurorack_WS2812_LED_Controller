# Menu Root
- Name: Breathing
  - Index: 0
  - Parameters
    - Speed
    - RGB color
- Name: Simple meter
  - Index: 1
  - Parameters
    - RGB color
    - Meter value
    - Flip
- Name: Mirrored meter
  - Index: 2
  - Parameters
    - RGB color
    - Meter value
    - Centered
- Name: Scroll rainbow
  - Index: 3
  - Parameters
    - Delta hue
    - Scroll hue
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