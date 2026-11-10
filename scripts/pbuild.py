import os
import shutil
import subprocess
import tempfile
import time
from multiprocessing import Pool


__VERSION = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).decode('utf-8').strip()

output = subprocess.check_output(["git", "status", "-s"]).decode('utf-8').strip()
if "^.M" in output:
	__VERSION = __VERSION

ARCH_FLAGS = "-m64 -march=x86-64 -mabi=sysv -mno-red-zone -mcmodel=kernel -MMD -MP"
WARN_FLAGS = "-Wall -Wextra -nostdlib"
STANDART_FLAGS = f"-std=gnu11 -DKERNEL_GIT_TAG=\\\"{__VERSION}\\\"" # -DNO_DEBUG=1
PROTECT_FLAGS = "-O0 -pipe -ffreestanding -fno-stack-protector -fdata-sections -fno-lto -fno-stack-check -fno-PIC -fno-PIE"
PROTECT_FLAGS += " -mno-mmx -mno-3dnow -mno-avx -ffunction-sections -fdata-sections"
CHARSET_FLAGS = "-finput-charset=UTF-8 -fexec-charset=cp1251" #""
LIBS_FLAGS = "-Ilimine -Iinclude -Imodlib"
FORMAT_CMD = """find . \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -print0 | xargs -0 clang-format -i -style=file"""

LIMINE_VERSION = "12.9.6"
LIMINE_RELEASE_URL = f"https://github.com/Limine-Bootloader/Limine/releases/download/v{LIMINE_VERSION}"
LIMINE_MARKER = os.path.join("limine", ".bmosp-limine-version")
LIMINE_REQUIRED_FILES = [
	"limine/limine.h",
	"limine/limine",
	"limine/limine.c",
	"limine/Makefile",
	"limine/limine-bios.sys",
	"limine/limine-bios-cd.bin",
	"limine/limine-uefi-cd.bin",
	"limine/BOOTX64.EFI",
	"limine/BOOTIA32.EFI",
]

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


def limine_is_current():
	try:
		with open(LIMINE_MARKER, "r") as file:
			if file.read().strip() != LIMINE_VERSION: return False
	except OSError:
		return False
	return all(os.path.isfile(path) for path in LIMINE_REQUIRED_FILES)


def download_file(url, path):
	subprocess.run(["curl", "-fL", "--retry", "2", "-o", path, url], check=True)


def check_limine():
	if limine_is_current():
		print(f"Limine {LIMINE_VERSION} уже установлен")
		return

	print(f"Установка Limine {LIMINE_VERSION}")
	temp_dir = tempfile.mkdtemp(prefix="bmosp-limine-")
	try:
		binary_archive = os.path.join(temp_dir, "limine-binary.tar.gz")
		source_archive = os.path.join(temp_dir, f"limine-{LIMINE_VERSION}.tar.gz")
		download_file(f"{LIMINE_RELEASE_URL}/limine-binary.tar.gz", binary_archive)
		download_file(f"{LIMINE_RELEASE_URL}/limine-{LIMINE_VERSION}.tar.gz", source_archive)
		subprocess.run(["tar", "-xzf", binary_archive, "-C", temp_dir], check=True)
		subprocess.run(["tar", "-xzf", source_archive, "-C", temp_dir], check=True)

		if os.path.isdir("limine"): shutil.rmtree("limine")
		os.makedirs("limine")
		shutil.copytree(os.path.join(temp_dir, "limine-binary"), "limine", dirs_exist_ok=True)
		shutil.copyfile(
			os.path.join(temp_dir, f"limine-{LIMINE_VERSION}", "limine-protocol", "include", "limine.h"),
			os.path.join("limine", "limine.h")
		)
		with open(LIMINE_MARKER, "w") as file: file.write(LIMINE_VERSION + "\n")
		subprocess.run(["make", "-C", "limine"], check=True)
	finally:
		shutil.rmtree(temp_dir, ignore_errors=True)

def create_iso(IMAGE_NAME):
	os.system(f"rm -f {IMAGE_NAME}.iso")
	os.system(f"rm -rf iso_root")
	os.system(f"mkdir -p iso_root")
	os.system(f"cp -v kernel.elf configs/limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin" \
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


	check_limine()

	major, minor, build = version_build()
	
	print("Сборка модульного ядра")
	compile_all()

	print("Создание ISO образа")
	create_iso("bmosp")

	print(f"Не забудьте сохранить изменения! Номер сборки: {major}.{minor}.{build}")
