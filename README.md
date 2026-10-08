<h1 align="center">
  <br>
  <img src="https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/3d_top_render.png" alt="docs/images/3d_top_render.png">

  <br>
  AuraForge-50X
  <br>
</h1>
<h3 align="center">
A fully open-sourced complete wireless sound computer. 
</h3>

| Front Render | Back Render | 
| --- | --- |
| ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/3d_top_render.png) | ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/3d_bottom_render.png) | 

## Schematics  

Here are the schematic design,

![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/schematic_preview.png)

## Features
- **ESP32-WROOM-32E MCU** - Dual-core Xtensa LX6, adjustable from 80 MHz to 240 MHz.
- **A 50Hz Wi-Fi CSI radar and real time DSP**.
- **TPA3116D2 Class-D amp**.
- **3A fast charging with BMS and Li-ion/LiPo both Battery support**.
- Power and battery indicator LED.
- Cyberpunk Web HUD.
- **USB C** - for programming.
- A 12-21V DC jack for power.

## PCB Design 
| ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/pcb_layout_2d.png) |
|--- |  


 ### The PCB is 4 layers with dimensions 100x80mm, made to be easily useable for anyone.  

 ### All the PCB files are [here](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/hardware) 

| Front Layer | Inner Layer 1 |
| --- | --- |
| ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/pcb_layout_top_layer.png) | ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/pcb_layout_2nd_layer.png) |
| Inner Layer 2 | Bottom layer |
| ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/pcb_layout_3nd_layer.png) | ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/pcb_layout_bottom_layer.png) |

## Pinout  

This is a general pinout 

| ![](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/docs/images/Pin_Outs.png) | 
| --- | 

(Credits to the KiCad EDA for KICAD 10, Espressif Systems for the MCU, RuView for CSI processing and Hack Club for fund to make this possible.) 

(Thanks to Madhav, tty7 to review my Design and tell me some mistakes like the antenna is pleased on board without open air due to that the range of it is low after the review from them I places the antenna in open air to get full range of Bluetooth and correct Wi-Fi CSI) 

## Why was it made?  

While using the normal Bluetooth amp for my many projects and in daily need, I realised it doesn't have Wi-Fi and high power support and I couldn't use it, unfortunately I had to think to buy a different high power amp then I see that those have high power but not Wi-Fi support and use very low price components which give low quality, but now I designed one with the simplicity of ESP32-WROOM-32E with being able to use Wi-Fi and Bluetooth both and also Programing support in a same board with high quality audio support for amp IC and have too many features.

## How you can get one for yourself? 

That is pretty easy, you have all the files you will need in the production folder, order the development  
board from any PCB fab ( I chose PCB Power cause its cheap in India and has good quality and also made in India ) and BINGO! You will have it to use for any purpose you want!  

## How do you use it?  
You can use it as any normal amp board but first want to flash the firmware which is given in firmware folder, for flashing you need to first enter the boootloader mode by holding the BOOT buttom and one time press and release the RESET button while connecting to your device, flash it with firmware and after all flashing done press the RESET button one time and you are good to go!
I have attached a simple firmware to start with prebuild firmware available in the firmware folder.  

## BOM  
|Name|Purpose          |Quantity|Total Cost (USD)|Link               |Distributor|
|----|-----------------|--------|----------------|-------------------|-----------|
|PCBA|Board and Assembly|2       |62.12           |[Lion Circuits](lioncircuits.com)|Lion Circuits     |
|Electronic Components|BOM Component Sourcing|2       |117.00           |[Lion Circuits](lioncircuits.com)|Lion Circuits     |
|2 Components|2 BOM Component Sourcing which is not got from Lion Circuits|2       |2.59           |[Sharvi Electronics](sharvielectronics.com)|Sharvi Electronics     |
|Battery|18650 3.7V 3000mAh Li-ion Battery with Connector for testing|1       |5.00          |[18650 Battery](https://www.amazon.in/gp/product/B0HBPWY1CS/ref=ox_sc_act_title_3?smid=AJWV8HAM5L3YL&psc=1)|Amazon India     |
|Speaker|5 Inch 4 Ohm 25W Full-Range Woofer Speaker (Stereo Pair) for testing|1       |5.48           |[5 Inch Speaker](https://www.amazon.in/gp/product/B0D8CVNZVK/ref=ox_sc_act_title_2?smid=A3C7Z4RVM9A3G1&psc=1)| Amazon India     |
|Data & Power Cable|100W USB-C to USB-C Silicone Cable (3ft / 1.0m) for uploading firmware|1       |5.00           |[USB-C Cable](https://www.amazon.in/gp/product/B0F316KC1Q/ref=ox_sc_act_title_4?smid=AJ6SIZC8YQDZX&psc=1)| Amazon India     |
|Speaker connection Cable|20 Meter speaker wiring Cable (64ft / 20.0m) for connecting the speakers and testing|1       |3.10           |[20 Meter Wire](https://www.amazon.in/gp/product/B0CHBFB5H2/ref=ox_sc_act_title_1?smid=A2LH9DQUAZIHP2&psc=1)| Amazon India     |
|Aux Cable|5 Meter Aux Cable for testing the 3.5 mm Aux jack and wired audio input system|1       |2.70           |[5 Meter Aux Cable](https://www.amazon.in/gp/product/B08XBFV4VR/ref=ox_sc_act_title_2?smid=A2LH9DQUAZIHP2&psc=1)| Amazon India     |
|Power Supply|20 Volt Power supply to run the board on its high power to test all things is working on its full power|1       |5.55           |[20V Power Supply](https://www.amazon.in/gp/product/B0D22W62W9/ref=ox_sc_act_title_1?smid=A2QFGJBWFSGOIU&psc=1)| Amazon India     |
|Cable to give power to power supply|1.5 Meter Power Cable to give power to power supply addapter due to it not have cable with it|1       |1.60           |[Main Power cable](https://www.amazon.in/gp/product/B0F9VD5NC5/ref=ox_sc_act_title_1?smid=AJ6SIZC8YQDZX&psc=1)| Amazon India     |
| | | Total | 209.85 |

LCSC & DigiKey BOM for the components on board is [here](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/components_bom.csv)
