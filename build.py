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
    if result.stdout.strip().lower() == 'already up to date':
        print('No updates, waiting for further update...')
        return False
    print('Repository updated')
    return True

def convert_md_to_html(md_file):
    # Получаем путь к папке "output"
    bin_path = os.path.join(os.getcwd(), 'output')

    # Получаем заголовок странцы из файла
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
    os.system(f"pandoc -s {md_file} -o {html_file} --metadata title=\"{title}\"")


def main():
    # Получаем путь к папке "output"
    bin_path = os.path.join(os.getcwd(), 'output')


    # Создаем папку "output", если она не существует
    os.makedirs(bin_path, exist_ok=True)
    while True:
        # Проверяем обновление репозитория Git
        update_repo()

        # Получаем список всех файлов в репозитории
        repo_files = os.listdir('.')

        # Фильтруем только MD файлы
        md_files = [file for file in repo_files if file.endswith('.md')]

        # Преобразуем каждый MD файл в HTML и копируем в папку "output"
        for md_file in md_files:
            convert_md_to_html(md_file)

        # Ожидаем 1 минуту перед следующей проверкой обновлений
        time.sleep(60)

if __name__ == '__main__':
    main()