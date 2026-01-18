# NekoOS 🐱

A simple, stupid, operating system written from scratch with a cat-themed interface.

## Features
- **Terminal shell** with command parsing
- **VGA text mode** display driver
- **Keyboard input** support
- **Games**: Snake and Neko (cat) game
- **GRUB multiboot** compliant
- **Reboot command** for system control

## Project Structure
```
neko-os/
├── src/
│   ├── boot/
│   │   └── multiboot.asm    # Bootloader with multiboot header
│   ├── kernel/
│   │   ├── main.c           # Kernel entry point
│   │   ├── vga.c/h          # VGA text mode driver
│   │   ├── io.asm/h         # Port I/O functions
│   │   ├── terminal/
│   │   │   ├── terminal.c   # Shell implementation
│   │   │   └── terminal.h
│   │   ├── keyboard/
│   │   │   ├── keyboard.c   # Keyboard driver
│   │   │   └── keyboard.h
│   │   └── game/
│   │       ├── game.c       # Neko (cat) game
│   │       ├── game.h
│   │       └── snake.c      # Snake game
├── iso/
│   └── boot/
│       └── grub/
│           └── grub.cfg     # GRUB configuration
├── build/                   # Compiled objects
├── isodir/                 # Temporary ISO directory
└── nekoos.iso              # Final bootable ISO
```

## Building & Running

### Prerequisites
- gcc (cross-compiler recommended but not required)
- nasm (assembler)
- grub-mkrescue or xorriso
- qemu-system-i386 (for emulation)

### Build Commands
```
# Build the kernel
make

# Create bootable ISO
make iso

# Run in QEMU
make run
# or
qemu-system-i386 -cdrom nekoos.iso

# Clean build files
make clean
```

## Shell Commands
```
help      - Show available commands
clear     - Clear the screen
snake     - Play Snake game
neko game - Play Neko (cat) game
neko      - Display ASCII cat
reboot    - Reboot the system
```

## Development Notes
- Written in C and x86 Assembly
- Uses GRUB as bootloader for simplicity
- No standard library (-nostdlib)
- 32-bit protected mode
- VGA text mode at 0xB8000

## TODO / Future Ideas
- [ ] Memory management
- [ ] Filesystem
- [ ] More games
- [ ] Sound support
- [ ] GUI mode
- [ ] Network support

## License
MIT License - Feel free to use, modify, and distribute!

---

*"Not just another toy OS, it has cats!"* 🐾

*Made with 🐱 by justdev-chris*
