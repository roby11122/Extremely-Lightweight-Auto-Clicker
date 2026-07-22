To achieve the raw minimal file size, this project uses:
* **No CRT Dependency / Custom Entry Point:** (If you removed the standard main entry point to bypass C-Runtime bloat).
* **Minimize Size Compiler Flags:** `/O1` or `/Os` execution optimization.
* **Linker Trimming:** Strict COMDAT folding (`/OPT:ICF`) and unreferenced data elimination (`/OPT:REF`).
* **Direct Win32 UI:** Pure `user32.dll` and `gdi32.dll` manipulation without standard libraries.
