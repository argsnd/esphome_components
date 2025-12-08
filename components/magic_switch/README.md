# Magic Switch Component

An ESPHome component for detecting pulses on a GPIO pin, originally designed to support the "magic switch" feature of the Sonoff Basic R4.

## Overview

This component monitors a GPIO pin for pulse signals and triggers automation actions when valid pulses are detected. It measures the duration of HIGH pulses on the pin and can filter them based on configurable timing thresholds and exclusion ranges.

## How It Works

The component uses interrupt-driven edge detection to accurately measure pulse widths:
1. Monitors both rising and falling edges on the specified GPIO pin
2. Measures the duration of HIGH pulses in microseconds
3. Applies filtering based on your configuration
4. Triggers the `on_switch` automation when a valid pulse is detected

## Configuration Variables

### Required
- **pin** (Required, Pin): The GPIO pin to monitor for pulses

### Optional
- **timeout** (Optional, Time): Minimum pulse duration to detect. Pulses shorter than this are ignored. Default: `12ms` (12000us)
- **ignore_pulse_min** (Optional, Time): Start of the pulse exclusion range. Default: `0us` (disabled)
- **ignore_pulse_max** (Optional, Time): End of the pulse exclusion range. Default: `0us` (disabled)
- **on_switch** (Optional, Automation): Actions to perform when a valid pulse is detected

## Usage Examples

### Basic Configuration
Detect pulses longer than 13ms on GPIO5 (typical for Sonoff Basic R4):
```yaml
magic_switch:
  pin: 5
  timeout: 13000us
  on_switch:
    - logger.log: "Magic switch activated!"
    - switch.toggle: relay
```

### With Pulse Exclusion Range
Detect pulses longer than 2ms, but ignore pulses specifically between 9ms and 10.1ms:
```yaml
magic_switch:
  pin: GPIO12
  timeout: 2000us
  ignore_pulse_min: 9000us
  ignore_pulse_max: 10100us
  on_switch:
    - logger.log: "Valid pulse detected"
    - light.toggle: my_light
```

**Pulse filtering behavior:**
- < 2000us: Ignored (below threshold)
- 2000-8999us: Detected
- 9000-10100us: Ignored (in exclusion range)
- > 10100us: Detected

### Multiple Actions on Detection
```yaml
magic_switch:
  pin: GPIO5
  timeout: 12000us
  on_switch:
    - logger.log: "Pulse detected"
    - homeassistant.event:
        event: esphome.magic_switch_triggered
    - script.execute: handle_switch
```

## Use Cases

### Noise Filtering
If your GPIO pin receives interference or unwanted signals at a specific frequency, use the ignore range to filter them out while still detecting valid control pulses.

### Multi-Protocol Detection
When working with devices that send different pulse patterns, you can selectively respond to specific pulse durations while ignoring others.

### Sonoff Basic R4
For the Sonoff Basic R4 "magic switch" feature, use GPIO5 with a timeout around 12-13ms. The device sends specific pulse patterns when the external switch is toggled.

## Technical Details

- Uses interrupt service routine (ISR) for accurate timing
- Pulse duration measured using `micros()` for microsecond precision
- All timing comparisons happen in the ISR for minimal latency
- Detected pulses are logged at DEBUG level with their duration

## Notes

- The component only measures HIGH pulse durations (time the pin is HIGH)
- Pulses are measured from rising edge to falling edge
- The ignore range feature is disabled when `ignore_pulse_max` is 0 or not configured
- All time values are specified in microseconds (us) or milliseconds (ms)

## Acknowledgement

All of this code, apart from the exclusion range feature, comes from https://github.com/ssieb/esphome_components/tree/master/components/magic_switch
