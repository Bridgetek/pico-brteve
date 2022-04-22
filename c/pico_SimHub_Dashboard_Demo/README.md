# pico SimHub Dashboard Demo

This is a simplified dashboard for use with SimHub. It will accept a custom message from SimHub.

Step 1: Open Arduino -> My Hardware

Step 2: Select **Multiple Arduinos**. Find the pico COM port appears in the Hardware scan.

Step 3: At new **MULTIPLE USB SETTINGS**:
 * Fill in Device Name “pico EVE Dash”. The baud rate is not used.
 * Edit the Custom protocol:
```
format([DataCorePlugin.GameData.NewData.SpeedMph]*10,'0') + ';' +
([DataCorePlugin.GameData.NewData.Rpms]) + ';' +
[DataCorePlugin.GameData.NewData.Gear] + ';E'
```
Step 4: Then Click the button **Apply Changes Now**.

By default the project will build for the BT817 as used in the IDM2040-7A module. To change this alter the FT8XX_TYPE in the CMakeLists.txt file.

```
# Modify these to set the target GPU and display
set(FT8XX_TYPE BT817)
set(DISPLAY_RES WVGA)
```
