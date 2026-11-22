# SkyRC Charger for Tessera

A Tessera plugin for SkyRC B500Neo/Q500Neo chargers. Each channel tile shows the live voltage, current, capacity and status, and provides separate start/stop buttons, a charging-program selector, a battery-type selector and a charge-current control.

## Set up

1. Add this plugin to the desired screen in Tessera and build its firmware.
2. In the screen's layout, add one `plugin:skyrc_charger.channel` tile per charger channel.
3. Choose the channel's voltage, current, capacity and status sensors; program and battery type selects; charge-current number; and that channel's distinct start and stop buttons.
4. For a Guition 4-inch screen, use the tile's 4×3 size. The supplied layout demonstrates all four channels with live-verified entity IDs.

Tapping the left and right halves of the bottom row invokes only the selected channel's Start or Stop button. Tapping the program or battery row advances its select by one available option. Tapping the current row increases charge current by its Home Assistant `step`, wrapping to `min` after `max`. These are Home Assistant actions; use the on-screen controls only when it is safe to operate the charger.

## Entity mapping and limits

The `werkstatt_b500neo` device exposes per-channel sensors, two buttons, two selects and a charge-current number. The example layout uses the exact IDs read from Home Assistant. On the live read, channels B–D reported idle, 0 V, 0 A and 0 mAh; channel A's telemetry was unavailable. The charge-current entity reported 100 mA, minimum 100 mA, maximum 10,000 mA and step 100 mA. Select options come from Home Assistant at runtime.

Not built or tested on hardware yet.
