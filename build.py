from bs4 import BeautifulSoup

import os
import subprocess
import shutil
import time


def update_repo():
    # Переходим в директорию с репозиторием
    repo_path = '.'
    os.chdir(repo_path)

    # Выполняем команду git pull для обновления репозитория
    result = subprocess.run(['git', 'pull'], capture_output=True, text=True)
    
    # Проверяем вывод команды git pull
    if 'up to date' in result.stdout.strip().lower():
        print('No updates, waiting for further update...')
        return False
    print('Repository updated')
    return True


def add_to_head(file_path, string):
    with open(file_path, 'r') as file:
        contents = file.read()
        soup = BeautifulSoup(contents, 'html.parser')
        head = soup.head
        
        # Парсим строку в виде HTML-кода с помощью Beautiful Soup
        string_soup = BeautifulSoup(string, 'html.parser')
        
        # Находим содержимое тега string_soup и добавляем его внутрь head
        for tag in string_soup.contents:
            head.append(tag)
        
    with open(file_path, 'w') as file:
        file.write(str(soup))


def remove_header(file_path):
    with open(file_path, 'r') as file:
        html = file.read()

    soup = BeautifulSoup(html, 'html.parser')
    header_tag = soup.find('header', id='title-block-header')
    if header_tag:
        header_tag.decompose()
        with open(file_path, 'w') as file:
            file.write(str(soup))


def convert_md_to_html(md_file):
    # Получаем путь к папке "output"
    bin_path = os.path.join(os.getcwd(), 'output')

    # Получаем заголовок страницы из файла
    title = ''
    with open(md_file, 'r') as file:
        for line in file:
            if line.startswith('#'):
                title = line.strip('#').strip()
                break

    # Генерируем имя файла HTML на основе имени файла MD
    file_name = os.path.splitext(md_file)[0] + '.html'
    html_file = os.path.join(bin_path, file_name)

    # Преобразуем файл MD в HTML, используя, например, Pandoc
    print(f"pandoc -s {md_file} -o {html_file} --metadata title=\"{title}\" --css=assets/css/main.css")
    os.system(f"pandoc -s {md_file} -o {html_file} --metadata title=\"{title}\" --css=assets/css/main.css")
    
    # Добавляем метатег в HTML
    add_to_head(html_file, """<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">""")

    # Удаляем заголовок с использованием функции remove_header
    remove_header(html_file)


def main():
    # Получаем путь к папке "output" и "assets"
    bin_path = os.path.join(os.getcwd(), 'output')
    assets_path = os.path.join(os.getcwd(), 'assets')


    while True:
        
        shutil.rmtree(bin_path, ignore_errors=True)
        os.makedirs(bin_path, exist_ok=True)

        shutil.copytree(assets_path, os.path.join(bin_path, 'assets'))

        # Получаем список всех файлов в репозитории
        repo_files = os.listdir('.')

        # Фильтруем только MD файлы
        md_files = [file for file in repo_files if file.endswith('.md')]

        # Преобразуем каждый MD файл в HTML и копируем в папку "output"
        for md_file in md_files:
            convert_md_to_html(md_file)

        shutil.rmtree('/var/www/html/', ignore_errors=True)
        shutil.copytree(bin_path, '/var/www/html/')

        # Проверяем обновление репозитория Git
        if not update_repo():
            time.sleep(60)


if __name__ == '__main__':
    main()