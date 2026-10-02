# Cardputer-Pokedex 📱🔴

A fast, 100% offline, standalone Pokédex built for the **M5Stack Cardputer** (ESP32-S3). Features physical keyboard searching by Pokémon name or National Dex ID, base stats display, typing, generation tags, and 64×64 pixel-art sprite rendering directly from a MicroSD card.

---

## 🌟 Features

* **Offline & Standalone:** Runs entirely on the ESP32 and MicroSD card—no Wi-Fi connection or external APIs required.
* **Hardware Keyboard Integration:** Instant search as you type using the Cardputer's mechanical keyboard.
* **Full National Dex Support:** Searches across generations 1 through 9.
* **On-the-Fly SD Parsing:** Reads Pokémon stats directly from `pokemon.csv` and streams 64×64 PNG/BMP sprites from disk.
* **Clean UI Layout:** Fits full stat breakdowns (HP, Atk, Def, SpA, SpD, Speed, BST) and typing cleanly on the 240×135 display.

---

## 🛠️ Hardware Requirements

* **M5Stack Cardputer**
* **MicroSD Card** (32GB or smaller recommended, formatted as FAT32)

---

## 📂 MicroSD Card Setup

Prepare your MicroSD card with the following structure at the root directory:

```text
E:\ (SD Card Root)
 ├── pokemon.csv           <-- Dataset containing Pokémon stats
 └── sprites\              <-- Folder containing sprites
      ├── 1.png
      ├── 2.png
      ├── 25.png
      └── ...
