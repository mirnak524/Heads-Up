# Heads-Up
**An ESP-NOW Visual Alert System**  
This low-power, wireless alert system is built on two ESP32 boards. Pressing one of the four buttons on a handheld sender will send a command over ESPNOW to a receiver across the room. Depending on the button input, the receiver will light up an RGB LED with a color tied to that button. This system is designed to get someone's attention when audio cues will not work. 
> Language: C++ (Arduino)  
> Platform: ESP32  
> Protocol: ESPNOW with P2P communication  
> Software Used: Autodesk Inventor

**Highlights**  
- Peer-to-peer wireless with ESPNOW: No router, pairing screen, and no internet dependency. The sender directly transmits to the receiver's MAC address on a fixed WiFi channel.
- Power Efficient: The sender remains in *deep sleep* most of the time until it's woken with a button press with an `ext0` GPIO wakeup. The receiver implements light sleep with intermittent idles to reduce power draw and mitigate heat build up in the 3Dprinted enclosure.
- Custom Enclosure: Designed in Autodesk Inventor and 3D printed. The switch plate is printed in Markforged Onyx for extra stability while the box/housing was done in PETG. 
- Each input button maps to a different color on the two RGB LEDs

## To-Do
- Web dashboard so the light can be accessed from different mobile devices
- Vibration motor in case the light is missed
