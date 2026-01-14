bits 16
org 0x7C00

start:
    cli                     ; Disable interrupts
    cld                     ; Clear direction flag
    
    ; Set up stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Save boot drive
    mov [boot_drive], dl
    
    ; Load kernel from disk
    mov ah, 0x02            ; Read sectors
    mov al, 15              ; Number of sectors to read
    mov ch, 0               ; Cylinder
    mov dh, 0               ; Head
    mov cl, 2               ; Sector (1 is boot sector)
    mov bx, 0x7E00          ; Load after bootloader
    int 0x13
    jc disk_error
    
    ; Jump to kernel
    jmp 0x7E00

disk_error:
    mov si, error_msg
    call print_string
    jmp $

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
    .done:
    ret

error_msg db "Disk error!", 0
boot_drive db 0

times 510-($-$$) db 0
dw 0xAA55