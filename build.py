import os
import time
import shutil
from git import Repo
from mistune import Markdown

# Путь до репозитория Git
repo_path = '/project/pages/'

# Путь до папки assets
assets_path = os.path.join(repo_path, 'assets')

# Путь до папки, в которую будут скопированы файлы
target_path = '/var/www/pages/'

# Функция для преобразования markdown в html
def markdown_to_html(markdown_content):
    markdown = Markdown()
    return markdown(markdown_content)

# Функция для копирования файлов и папки
def copy_files(source_path, target_path):
    if os.path.isdir(source_path):
        shutil.copytree(source_path, target_path)
    else:
        shutil.copy2(source_path, target_path)

while True:
    try:
        # Открываем репозиторий
        repo = Repo(repo_path)

        # Проверяем наличие изменений
        if repo.is_dirty():
            # Вытягиваем изменения из репозитория
            origin = repo.remote(name='origin')
            origin.pull()

            # Получаем список markdown файлов
            markdown_files = [file for file in os.listdir(repo_path) if file.endswith('.md')]

            # Копируем файлы и папку assets в целевую папку
            for file in markdown_files:
                source_file_path = os.path.join(repo_path, file)
                target_file_path = os.path.join(target_path, file.replace('.md', '.html'))
                with open(source_file_path) as f:
                    markdown_content = f.read()
                    html_content = markdown_to_html(markdown_content)
                    with open(target_file_path, 'w') as f:
                        f.write(html_content)
            copy_files(assets_path, target_path)

        # Засыпаем на 10 минут
        time.sleep(600)

    except Exception as e:
        print(f'Error: {e}')
        time.sleep(600)