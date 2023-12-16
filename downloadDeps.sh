#!/bin/bash

# CppPotpourri (C3P)
rm -rf lib/CppPotpourri
git clone --depth=1 https://github.com/jspark311/CppPotpourri.git lib/CppPotpourri

# ManuvrPlatform
rm -rf lib/ManuvrPlatform lib/Platform
git clone --depth=1 https://github.com/jspark311/ManuvrPlatform lib/ManuvrPlatform
ln -s lib/Platform lib/ManuvrPlatform/Linux   # Link in Linux as the operating build tree.
