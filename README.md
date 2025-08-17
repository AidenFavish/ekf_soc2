# EKF-SoC (C++)

C++ implementation of an Extended Kalman Filter for battery State‑of‑Charge estimation.

## Requirements
- CMake ≥ 3.16
- C++17 compiler

## Quick Start (Unix)
```bash
# from repo root
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j
./build/ekf_soc_app
```