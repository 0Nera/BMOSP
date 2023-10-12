# Генерируем код для isr_stubs
isr_stubs_code = ''
for i in range(256):
    isr_stubs_code += f'\t.quad isr_stub_{i}\n'

# Генерируем код для isr_stub
isr_stub_code = ''
for i in range(256):
    isr_stub_code += f'stub {i}\n'

# Сохраняем код в файл
with open('output.s', 'w') as file:
    file.write('.section .text\n')
    file.write(isr_stub_code)
    file.write('.global isr_stubs\n')
    file.write('.section .data\n')
    file.write('isr_stubs:\n')
    file.write(isr_stubs_code)