# HZKonverter – Conversor de Sample Rate 44.1 kHz ↔ 48 kHz

**HZKonverter** es un plugin de audio (VST3 y Standalone) escrito en **C++17** utilizando el framework **JUCE**.  
Permite convertir archivos de audio entre **44.1 kHz** y **48 kHz** sin alterar la fase ni afectar la integridad del sonido.

Este proyecto nace con el objetivo de aportar una herramienta simple y útil para productores musicales y, al mismo tiempo, servir como parte del portafolio profesional del autor en el área de C++/DSP/DevOps.

---

## 🚀 Características

- Detección automática del *sample rate* de entrada.
- Conversión:
  - **44.1 kHz → 48 kHz**
  - **48 kHz → 44.1 kHz**
- Interfaz gráfica con:
  - Zona “drag & drop”
  - Indicadores de estado
  - Logos personalizados
- Opción para sobrescribir archivo original o guardar una copia nueva.
- Compatibilidad con formatos:
  - WAV, AIFF, FLAC, MP3 (lectura)
- Implementación en C++ con JUCE + CMake.

---

## 🛠️ Tecnologías

- **C++17**
- **JUCE** (GUI, Audio, archivos)
- **CMake**
- **Visual Studio 2022**

---

## 📦 Cómo compilar en Windows

```bash
git clone https://github.com/josueqagt/HZKonverter.git
cd HZKonverter
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
