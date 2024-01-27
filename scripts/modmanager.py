import os
import json
import requests
import shutil
import zipfile
import argparse

module_path = "modules/modules.json"
loader_config_path = "configs/autorun.cfg"

def download_module(module_url, module_name):
    response = requests.get(module_url)
    file_path = os.path.join('modules', module_name)
    with open(file_path, 'wb') as file:
        file.write(response.content)
    return file_path

def extract_module(module_path, destination_folder):
    with zipfile.ZipFile(module_path, 'r') as zip_ref:
        zip_ref.extractall(destination_folder)
    return destination_folder

def install_module(module):
    module_name = module['name']
    module_url = module['url']
    module_zip_path = download_module(module_url, module_name)
    module_folder_path = os.path.join('modules', module_name)
    extract_module(module_zip_path, module_folder_path)
    with open(loader_config_path, 'a') as config_file:
        config_file.write(f"\nMODULE_PATH=boot://mod/{module_name}.ko\n")
        config_file.write(f"MODULE_CMDLINE=[MOD]{module_name}.ko\n")
    os.remove(module_zip_path)

def remove_module(module):
    module_name = module['name']
    module_folder_path = os.path.join('modules', module_name)
    shutil.rmtree(module_folder_path)
    with open(loader_config_path, 'r') as config_file:
        lines = config_file.readlines()
    new_lines = []
    for line in lines:
        if not line.strip().startswith("MODULE_PATH=") and (f"[MOD]{module_name}.ko" not in line and f"{module_name}.ko" not in line):
            new_lines.append(line)
    with open(loader_config_path, 'w') as config_file:
        config_file.writelines(new_lines)

def update_module_list(repo_url):
    response = requests.get(repo_url)
    response.raise_for_status()
    module_list = response.json()
    with open(module_path, 'w') as file:
        json.dump(module_list, file)

def install_selected_module(module_list, module_index):
    if 0 <= module_index < len(module_list):
        install_module(module_list[module_index])
    else:
        print("Неправильный номер!")

def remove_selected_module(module_list, module_index):
    if 0 <= module_index < len(module_list):
        remove_module(module_list[module_index])
    else:
        print("Неправильный номер!")

def handle_module_actions(module_list):
    while True:
        print("1. Установить модуль")
        print("2. Удалить модуль")
        print("3. Выход")
        choice = input("[MOD]: ")
        if choice == "1":
            print("Доступные модули:")
            for i, module in enumerate(module_list, start=0):
                print(f"{i}. {module['name']}")
            module_index = int(input("Введите номер модуля: "))
            install_selected_module(module_list, module_index)
        elif choice == "2":
            print("Установленные модули:")
            for i, module in enumerate(module_list, start=1):
                module_folder_path = os.path.join('modules', module['name'])

                if os.path.exists(module_folder_path):
                    print(f"{i}. {module['name']}")
            module_index = int(input("Введите номер модуля для удаления: ")) - 1
            remove_selected_module(module_list, module_index)
        elif choice == "3":
            break


def main():

    with open(module_path) as file:
        repositories = json.load(file)
    for repo in repositories:
        repo_list_url = repo['list']
        update_module_list(repo_list_url[0])

    with open(module_path) as file:
        module_list = json.load(file)
        handle_module_actions(module_list)

if __name__ == "__main__":
    main()
