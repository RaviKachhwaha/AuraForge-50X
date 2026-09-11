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
|PCBA|Board and Assembly|3       |111.32           |https://www.pcbpower.com/|PCB Power     |
|Electronic Components|BOM Component Sourcing|2       |57.88           |https://www.digikey.in/|DigiKey     |
|Battery|18650 3.7V 3000mAh Li-ion Battery with Connector for testing|1       |6.08           |https://www.amazon.in/Charging-18650-Rechargeable-Connector-Protection/dp/B0DBR36C91|Amazon India     |
|Speaker|5 Inch 4 Ohm 25W Full-Range Woofer Speaker (Stereo Pair) for testing|1       |8.16           |https://www.amazon.in/-/hi/Electronic-Spices-%E0%A4%95%E0%A5%88%E0%A4%AC%E0%A4%BF%E0%A4%A8%E0%A5%87%E0%A4%9F-%E0%A4%B0%E0%A4%BF%E0%A4%AA%E0%A5%8D%E0%A4%B2%E0%A5%87%E0%A4%B8%E0%A4%AE%E0%A5%87%E0%A4%82%E0%A4%9F-%E0%A4%A1%E0%A5%8D%E0%A4%B0%E0%A4%BE%E0%A4%87%E0%A4%B5%E0%A4%B0/dp/B0BN44KVVL| Amazon India     |
|Data & Power Cable|100W USB-C to USB-C Silicone Cable (3ft / 1.0m) for uploading firmware|1       |8.24           |https://www.amazon.in/-/hi/Anker-A8552/dp/B093GGVB89| Amazon India     |
| | | Total | 191.68 | 

LCSC BOM for the components on board is [here](https://github.com/RaviKachhwaha/AuraForge-50X/blob/main/components_bom.csv)
