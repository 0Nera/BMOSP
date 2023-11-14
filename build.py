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

def remove_header(file_path):
    with open(file_path, 'r') as file:
        html = file.read()

    start_tag = '<header id="title-block-header">\n<h1 class="title">'
    end_tag = '</h1>\n</header>'

    start_index = html.find(start_tag)
    end_index = html.find(end_tag) + len(end_tag)

    if start_index != -1 and end_index != -1:
        html = html[:start_index] + html[end_index:]

    with open(file_path, 'w') as file:
        file.write(html)

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
    print(f"pandoc -s {md_file} -o {html_file} --metadata title=\"{title}\" --css=assets/css/main.css")
    os.system(f"pandoc -s {md_file} -o {html_file} --metadata title=\"{title}\" --css=assets/css/main.css")
    remove_header(html_file)


def main():
    # Получаем путь к папке "output" и "assets"
    bin_path = os.path.join(os.getcwd(), 'output')
    assets_path = os.path.join(os.getcwd(), 'assets')


    while True:
        # Проверяем обновление репозитория Git
        if not update_repo():
            time.sleep(60)
        
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
        
        # Ожидаем 2 минуты перед следующей проверкой обновлений
        time.sleep(120)

if __name__ == '__main__':
    main()