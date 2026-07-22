# 🖱️ Extremely Lightweight Auto-Clicker

A high-performance, hyper-optimized Windows auto-clicker built with pure **Win32 C++**. By completely dropping the C Runtime Library (CRT) dependency and avoiding heavy automation frameworks, this entire application compiles down to an impossible **13 KB standalone executable**.

No installation, no telemetry, no framework overhead—just raw machine code.

## ⚡ Performance Breakdown

Most modern utilities drag in megabytes of browser engines or styling layers. This project strips computing back to its fundamental roots.

| Metric | Most Auto-Clickers (OP/Others) | This Project | Improvement |
| :--- | :--- | :--- | :--- |
| **Executable Size** | 800 KB - 15 MB | **13 KB** | ⬇️ ~98% Smaller |
| **RAM Footprint** | 20 MB - 60 MB | **< 2.5 MB** | ⬇️ ~95% Lighter |
| **GPU Utilization** | 1% - 5% (Hardware Accel.) | **0.0%** (Direct Win32) | 🎯 Bypassed Completely |
| **Dependencies** | .NET / VC++ Redist / Python | **None** (Pure Native) | 📦 Zero Install |


## ✨ Features

* **Zero-Overhead GUI:** Native Windows interface utilizing raw `user32.dll` and `gdi32.dll` layout widgets.
* **On-the-Fly Dynamic Rebinding:** Change your global start/stop hotkeys instantly in real-time.
* **Smart Thread Sleeping:** Utilizes high-precision kernel-level `SetWaitableTimer` handles to ensure **0% CPU lockups**, even on legacy single-core hardware.
* **Persistent Configuration:** Automatically saves your custom hotkey binds directly to a hyper-compact local configuration index (`c.ini`).


## 🛠️ How It Was Achieved (Micro-Optimizations)

To achieve the 13KB footprint without losing features, the source code bypasses conventional modern compiler structures:
1. **Bypassing the C Runtime (CRT):** Compiled with `/NODEFAULTLIB` and a custom entry point hook (`mainCRTStartup`). This completely erases internal compiler boilerplate weight.
2. **Hand-Rolled Core Utilities:** Standard library string, allocation, and buffer routines (`strlen`, `itoa`, `strcat`) were written from scratch inline.
3. **Strict Linker Trimming:** Built using intensive optimization flags including size minimization (`/O1`), explicit COMDAT folding (`/OPT:ICF`), and dead-code elimination (`/OPT:REF`).

## 📝 License

This project is open-source and available under the [MIT License](LICENSE). Feel free to fork, min-max, or modify!
