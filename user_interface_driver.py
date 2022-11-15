#Author:Diego Silva
#Date:15/11/2022
#Description:Software to write and read operation in kernel

import os

def conexao_driver():
	return os.open("/dev/mechadog", os.O_RDWR)

def escrita(conexao):
	valor = input("Valor")
	os.write(conexao,valor.encode())
	print(os.read(conexao,len(valor)))
def leitura():
	pass
	#os.system("cat > /dev/mechadog")




print("--------------------------------------------")
print("----------------Bem vindo-------------------")
print("--------------------------------------------")
conexao = conexao_driver()
escrita(conexao)
#leitura()
