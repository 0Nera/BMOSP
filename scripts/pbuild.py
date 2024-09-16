import os
import shutil
import subprocess
import time
from multiprocessing import Pool


__VERSION = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).decode('utf-8').strip()

output = subprocess.check_output(["git", "status", "-s"]).decode('utf-8').strip()
if "^.M" in output:
	__VERSION = __VERSION

ARCH_FLAGS = "-m64 -march=x86-64 -mabi=sysv -mno-red-zone -mcmodel=kernel -MMD -MP"
WARN_FLAGS = "-Wall -Wextra -nostdlib"
STANDART_FLAGS = f"-std=gnu11 -DKERNEL_GIT_TAG=\\\"{__VERSION}\\\"" # -DNO_DEBUG=1
PROTECT_FLAGS = "-O0 -g -pipe -ffreestanding -fno-stack-protector -fno-lto -fno-stack-check -fno-PIC -fno-PIE"
PROTECT_FLAGS += " -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx"
CHARSET_FLAGS = "-finput-charset=UTF-8 -fexec-charset=cp1251"
LIBS_FLAGS = "-Ilimine -Iinclude"
FORMAT_CMD = """find . \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -print0 | xargs -0 clang-format -i -style=file"""

def version_build():
	with open("include/version.h", "r") as file:
		lines = file.readlines()

	major = 0
	minor = 0
	build = 0

	with open("include/version.h", "w") as file:
		for line in lines:
			if line.startswith("#define VERSION_BUILD"):
				parts = line.split()
				build = int(parts[2]) + 1
				if build > 999:
					build = 0
					minor += 1
			elif line.startswith("#define VERSION_MAJOR"):
				parts = line.split()
				major = int(parts[2])
			elif line.startswith("#define VERSION_MINOR"):
				parts = line.split()
				minor += int(parts[2]) 
			else:
				file.write(line)
		file.write(f"#define VERSION_MAJOR {major}\n")
		file.write(f"#define VERSION_MINOR {minor}\n")
		file.write(f"#define VERSION_BUILD {build}\n")
	return [major, minor, build]

def sort_strings(strings):
	return sorted(strings, key=lambda x: not x.endswith('.s.o'))

def find_files(directory, extensions):
	file_list = []
	for root, dirs, files in os.walk(directory):
		for file in files:
			if any(file.endswith(extension) for extension in extensions):
				file_list.append(os.path.join(root, file))
	return file_list


def compile(file: str):
	CC = "gcc"
	if os.path.isdir("sdk"):
		CC = "./sdk/bin/x86_64-elf-gcc"
	output_file = file.replace('/', '_')
	obj_file = f"bin/{output_file}.o"
	cmd = f"{CC} {WARN_FLAGS} {STANDART_FLAGS} {PROTECT_FLAGS} {ARCH_FLAGS} {CHARSET_FLAGS} {LIBS_FLAGS} -c {file} -o {obj_file}"
	print(cmd)
	os.system(cmd)
	return obj_file


def compile_all():
	file_list = find_files("kernel/", [".s", ".cpp", ".c"])
	file_list += find_files("kernel/*/*", [".s", ".cpp", ".c"])

	with Pool() as pool:
		results = pool.map(compile, file_list)

	while not all(results):
		print(results)
		time.sleep(1)
	print(results)
	cmd = f"ld -nostdlib -static -m elf_x86_64 -z max-page-size=0x1000 -T configs/linker.ld -o kernel.elf {' '.join(sort_strings(results))}"
	print(cmd)
	os.system(cmd)


def check_limine():
	if not os.path.isdir("limine"):
		subprocess.run(["git", "clone", "https://git.synapseos.ru/Aren/limine.git", "--branch=v5.x-branch-binary", "--depth=1"])
	else:
		os.chdir("limine")
		subprocess.run(["git", "pull"])
		os.chdir("..")
	os.chdir("limine")
	subprocess.run(["make"])
	os.chdir("..")

def create_hdd(IMAGE_NAME):
	os.system(f"rm -f {IMAGE_NAME}.hdd".format())
	os.system(f"dd if=/dev/zero bs=1M count=0 seek=4 of={IMAGE_NAME}.hdd")
	os.system(f"sgdisk {IMAGE_NAME}.hdd -n 1:2048 -t 1:ef00")
	os.system(f"./limine/limine bios-install {IMAGE_NAME}.hdd")
	os.system(f"mformat -i {IMAGE_NAME}.hdd@@1M")
	os.system(f"mmd -i {IMAGE_NAME}.hdd@@1M ::/EFI ::/EFI/BOOT")
	os.system(f"mcopy -i {IMAGE_NAME}.hdd@@1M kernel.elf configs/limine.cfg limine/limine-bios.sys ::/")
	os.system(f"mcopy -i {IMAGE_NAME}.hdd@@1M iso_root/mod/ ::/")
	os.system(f"mcopy -i {IMAGE_NAME}.hdd@@1M limine/BOOTX64.EFI limine/BOOTIA32.EFI ::/EFI/BOOT")
	os.system(f"./limine/limine bios-install {IMAGE_NAME}.hdd")


def create_iso(IMAGE_NAME):
	os.system(f"rm -f {IMAGE_NAME}.iso")
	os.system(f"rm -rf iso_root")
	os.system(f"mkdir -p iso_root")
	os.system(f"cp -v kernel.elf configs/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin" \
		   " limine/limine-uefi-cd.bin iso_root/")
	os.system(f"mkdir -p iso_root/EFI/BOOT")
	shutil.copytree("modules/bin", "iso_root/mod")
	os.system(f"cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/")
	os.system(f"cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/")
	os.system(f"xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table"\
		   f" --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso_root -o {IMAGE_NAME}.iso")
	os.system(f"./limine/limine bios-install {IMAGE_NAME}.iso")

if __name__ == "__main__":
	print("Форматирование кода")

	os.chdir("include")
	os.system(FORMAT_CMD)
	os.chdir("../kernel/")
	os.system(FORMAT_CMD)
	os.chdir("../modlib/")
	os.system(FORMAT_CMD)
	os.chdir("../modules/")
	os.system(FORMAT_CMD)
	os.chdir("../")


	print("Очистка папки bin")
	subprocess.run(["rm", "-rf", "bin"])
	subprocess.run(["mkdir", "-p", "bin"])

	if not os.path.isdir("ovmf"):
		print("Установка UEFI")
		subprocess.run(["mkdir", "-p", "ovmf"])
		os.chdir("ovmf")
		subprocess.run(["curl", "-Lo", "OVMF.fd", "https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd"])
		os.chdir("..")


	if not os.path.isdir("limine"):
		print("Установка Limine")
		check_limine()

	major, minor, build = version_build()
	
	print("Сборка модульного ядра")
	compile_all()

	print("Создание ISO образа")
	create_iso("bmosp")

	print("Создание HDD образа")
	create_hdd("bmosp")

	print(f"Не забудьте сохранить изменения! Номер сборки: {major}.{minor}.{build}")
