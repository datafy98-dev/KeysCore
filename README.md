# Hello World!

### *KeysCore (KeysMOS)* - This is the operating system for the *KeysCore* synthesizer.

## Device specifications
> ESP32 S3
> [!WARNING]
> Recommended version: N16R8
>
> Display 160x128 pixels
>
> 5 potanceometers
>
> Keyboard connector (MIDI/UART)
>
> Some GPIO inputs and outputs, including UART0

## How to use?
> Maybe later

## How to build?
> [!NOTE]
> Currently only ESP32 S3 is supported

<details>

<summary>- Option 1 (using console)</summary>

> 1. Download or clone the repository
```bash
git clone https://github.com/datafy98-dev/KeysCore.git 
cd keyscore
```
> 3. Make sure you have ESP-IDF installed
> 
> 4. Connect your device
> 
> 4. Run the command: "idf.py build flash monitor"
```bash
idf.py build flash monitor
```
> 5. Done

</details>

<details>


<summary>- Option 2 (Using "Code - OSS" or "Visual Studio Code")</summary>

> 1. Download or clone the repository
```bash
git clone https://github.com/datafy98-dev/KeysCore.git 
cd keyscore
``` 
> 2. Make sure you have ESP-IDF installed
> 
> 3. Connect your device
> 
> 4. Click on the button "ESP-IDF: Build, Flash and Monitor"
>
> 5. Done
</details>

### What's currently in development plans?
> - Potentiometer support
>
> - Multiple menu screens
>
> - Sound output and reception on PC client
