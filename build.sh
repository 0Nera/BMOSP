#!/bin/bash

CC="g++"
ARCH_FLAGS="-m64 -march=x86-64 -mabi=sysv -mno-80387 -mno-red-zone -mcmodel=kernel -MMD -MP"
WARN_FLAGS="-Wall -Wextra"
STANDART_FLAGS="-std=gnu11"
PROTECT_FLAGS="-O0 -pipe -ffreestanding -fno-stack-protector -fno-lto -fno-stack-check -fno-PIC -fno-PIE"
CHARSET_FLAGS="-finput-charset=UTF-8 -fexec-charset=cp1251"
LIBS_FLAGS="-Ilimine"

find_files() {
    file_list=()
    directory=$1
    extensions=$2
    while IFS= read -r -d '' file; do
        file_list+=("$file")
    done < <(find "$directory" -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.s" \) -print0)
    printf '%s\n' "${file_list[@]}"
}

compile() {
    file=$1
    output_file="${file/\//-}"
    obj_file="bin/${output_file}.o"
    cmd="$CC $WARN_FLAGS $PROTECT_FLAGS $ARCH_FLAGS $CHARSET_FLAGS $LIBS_FLAGS -c $file -o $obj_file"
    echo "$cmd"
    eval "$cmd"
    echo "$obj_file"
}

compile_all() {
    file_list=$(find_files "kernel/" ".c .cpp .s")
    IFS=$'\n' read -rd '' -a file_array <<<"$file_list"

    for file in "${file_array[@]}"; do
        compile "$file"
    done

    while [[ $(grep -o "bin/.*\.o" <<<"${results[*]}") ]]; do
        sleep 1
    done

    cmd="ld -nostdlib -static -m elf_x86_64 -z max-page-size=0x1000 -T configs/linker.ld -o kernel.elf ${results[*]}"
    echo "$cmd"
    eval "$cmd"
}

check_limine() {
    if [ ! -d "limine" ]; then
        git clone "https://github.com/limine-bootloader/limine.git" --branch=v5.x-branch-binary --depth=1
    else
        pushd "limine"
        git pull
        popd
    fi
    pushd "limine"
    make
    popd
}

check_tools() {
    required_tools=("g++" "xorriso" "make" "mtools")
    missing_tools=()

    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done

    if (( ${#missing_tools[@]} > 0 )); then
        sudo apt install "${missing_tools[@]}"
    fi
}

create_iso() {
    IMAGE_NAME=$1
    rm -rf "iso_root"
    mkdir -p "iso_root"
    cp -v "boot/CYRILL2.F16" "iso_root/"
    cp -v "kernel.elf" "configs/limine.cfg" "limine/limine-bios.sys" "limine/limine-bios-cd.bin" "limine/limine-uefi-cd.bin" "iso_root/"
    mkdir -p "iso_root/EFI/BOOT"
    cp -v "limine/BOOTX64.EFI" "iso_root/EFI/BOOT/"
    cp -v "limine/BOOTIA32.EFI" "iso_root/EFI/BOOT/"
    xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso_root -o "${IMAGE_NAME}.iso"
    ./limine/limine bios-install "${IMAGE_NAME}.iso"
    rm -rf "iso_root"
}

create_hdd() {
    IMAGE_NAME=$1
    rm -f "${IMAGE_NAME}.hdd"
    dd if=/dev/zero bs=1M count=0 seek=64 of="${IMAGE_NAME}.hdd"
    sgdisk "${IMAGE_NAME}.hdd" -n 1:2048 -t 1:ef00
    ./limine/limine bios-install "${IMAGE_NAME}.hdd"
    mformat -i "${IMAGE_NAME}.hdd@@1M"
    mmd -i "${IMAGE_NAME}.hdd@@1M" "::/EFI" "::/EFI/BOOT"
    mcopy -i "${IMAGE_NAME}.hdd@@1M" "kernel.elf" "configs/limine.cfg" "boot/CYRILL2.F16" "limine/limine-bios.sys" "::/"
    mcopy -i "${IMAGE_NAME}.hdd@@1M" "limine/BOOTX64.EFI" "limine/BOOTIA32.EFI" "::/EFI/BOOT"
}

check_limine
check_tools
compile_all
create_iso "MSEOS"
create_hdd "MSEOS"