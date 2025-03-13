# Better Control 🛠️ 
A GTK4-based control panel for Linux 🐧

Whats new:
- Rewritten in C for better performance
- Modern GTK4 UI
- Independently change volume of apps
- Fixed some scaling bugs
- Fixed Bluetooth bug where it showed both disconnect and connect

<img src="https://github.com/user-attachments/assets/5f9fe1eb-6bff-428a-95d2-5d97f8d4e209" width="500" />

This project is still under development, contributions such as ideas and feature requests towards project and testers are welcome.

# How to Install? ✅ 
Before install make sure you have `git` and `base-devel` installed

## Dependencies

- GTK 4 (for the UI)
- NetworkManager (for managing Wi-Fi & Ethernet)
- BlueZ & BlueZ Utils (for Bluetooth support)
- PipeWire Pulse (for audio control)
- Brightnessctl (for screen brightness control)
- D-Bus development libraries
- C compiler (gcc)

### Arch Based
Arch users don't need to do all this installation processes, 

If you use `yay` then:
```bash
yay -S better-control-git
```

If you use `paru` then:
```bash
paru -S better-control-git
```

### Debian Based
```bash
sudo apt update && sudo apt install -y libgtk-4-dev network-manager bluez bluez-utils pipewire-pulse brightnessctl gcc make libdbus-1-dev libdbus-glib-1-dev
```

### Fedora Based
```bash
sudo dnf install -y gtk4-devel NetworkManager bluez bluez-utils pipewire-pulse brightnessctl gcc make dbus-devel dbus-glib-devel
```

### Void Linux
```bash
sudo xbps-install -S gtk4-devel NetworkManager bluez bluez-utils pipewire-pulse brightnessctl gcc make dbus-devel dbus-glib-devel
```

### Alpine Linux
```bash
sudo apk add gtk4-dev networkmanager bluez bluez-utils pipewire-pulse brightnessctl gcc make dbus-dev dbus-glib-dev
```

## After you get the dependencies 
```bash
git clone https://github.com/quantumvoid0/better-control
cd better-control
sudo make install
sudo rm -rf ~/better-control
```

# How to uninstall? ❌

For arch users who installed through AUR do this to uninstall:
```bash
sudo pacman -Rns better-control-git
```

For others who installed with makefile follow the lines below:
```bash
git clone https://github.com/quantumvoid0/better-control
cd better-control
sudo make uninstall
sudo rm -rf ~/better-control
```

# Compatibility 📄
I have only tested this on Arch Linux with Hyprland, GNOME & KDE Plasma so testers are welcome to test it out and share their review in discussions/issues. This should work on all distros (if you tested it please leave a comment for any issues)

Probably will work on the stuff below:
| **Category**         | **Requirements**                                                                 |
|---------------------|----------------------------------------------------------------------------------|
| **Operating System**| Linux                                                                            |
| **Distributions**   | Arch based, Fedora Based, Debian Based, Void, Alpine                            |
| **Desktop Environments** | GNOME (tested), XFCE, KDE Plasma (tested with GTK support), LXDE/LXQT, etc.    |
| **Window Managers** | Hyprland (tested), Sway, i3, Openbox, Fluxbox, etc.                            |
| **Display Protocol**| Wayland (recommended), X11 (partial functionality)                               |

