#!/usr/bin/python
from elftools.elf.elffile import ELFFile

filename = 'binary_function.o'

#print('In file:', filename)
binary_code = b''
with open(filename, 'rb') as f:
    elffile = ELFFile(f)
    for section in elffile.iter_sections():
        if section and section.name in [".text", ".data", ".bss", ".rodata.str1.4"]:
            #print('  ' + section.name)
            binary_code = binary_code + section.data()
    with open("payload", "wb") as f:
        f.write(binary_code)
