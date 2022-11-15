/*
Author:Diego Silva
Date:15/11/2022
Description:Simple Linux Device Driver
*/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

//definições para classe e device
#define DEVICE_NAME "mechadog"
#define CLASS_NAME "mdog"

//informativos gerais do driver
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Diego Silva <diegolopestec2@gmail.com>");
MODULE_DESCRIPTION("A Simple Linux Device Driver");
MODULE_VERSION("1.0");

static int majorNumber;
static char message[256]={0};
static short size_of_message;
static int numberOpens=0;
static struct class* mechadogClass = NULL;
static struct device* mechadogDevice = NULL;

//prototipagem de metodos
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

//definindo estrutura de operações do device driver
static struct file_operations fops={
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release
};


//método de inicialização do device driver
static int __init mecha_dog_init(void){
	printk(KERN_INFO "MechaDog: Inicializando o Mecha Dog\n");

	//tento alocar o major number
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(majorNumber<0){
		printk(KERN_ALERT "Falha ao tentar alocar um major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "MechaDog: Major Number alocado com sucesso!!!\n");

	//registrando a classe do dispositivo
	mechadogClass = class_create(THIS_MODULE,CLASS_NAME);
	if(IS_ERR(mechadogClass)){
		unregister_chrdev(majorNumber,DEVICE_NAME);
		printk(KERN_ALERT "Falha ao registrar a classe do driver!!!\n");
		return PTR_ERR(mechadogClass);
	}
	printk(KERN_INFO "MechaDog: Classe registrada com sucesso");

	//registrando dispositivo
	mechadogDevice = device_create(mechadogClass,NULL,MKDEV(majorNumber,0),NULL,DEVICE_NAME);
	if(IS_ERR(mechadogDevice)){
		class_destroy(mechadogClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Falha ao criar dispositivo!!\n");
		return PTR_ERR(mechadogDevice);
	}
	printk(KERN_INFO "MechaDog: dispositivo registrado com sucesso!\n");
	return 0;
}


static void __exit mecha_dog_exit(void){
	device_destroy(mechadogClass, MKDEV(majorNumber,0));
	class_unregister(mechadogClass);
	class_destroy(mechadogClass);
	unregister_chrdev(majorNumber,DEVICE_NAME);
	printk(KERN_INFO "MechaDog: Driver removido com sucesso");
}

static int dev_open(struct inode *inodep , struct file *filep){
	numberOpens++;
	printk(KERN_INFO "MechaDog: Dispositivo foi acessado %d  vezes",numberOpens);
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "MechaDog: Dispositivo fechado !\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int erro_contador=0;
	int i;
	//compiando conteudo para o buffer interno
	erro_contador = copy_to_user(buffer,message,size_of_message);

	printk(KERN_INFO "MechaDog: Conteudo passado >>");
	if(erro_contador==0){
		for(i=0;i<size_of_message;i++){
			printk(KERN_INFO "%c",message[i]);
		}
		
		return (size_of_message);
	}
	else{
		printk(KERN_INFO "MechaDog: Falha ao registrar conteudo escrito pelo usuario");
		return -EFAULT;
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	sprintf(message,"%s (%zu letters)",buffer,len);
	size_of_message = strlen(message);
	printk(KERN_INFO "MechaDog: Escrito %zu caracteres",len);
	return len;
}

module_init(mecha_dog_init);
module_exit(mecha_dog_exit);
