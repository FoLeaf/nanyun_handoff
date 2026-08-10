#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	while (1) {
		printk("Hello World from STM32G070CBT6 on Zephyr RTOS (Clang/LLVM)!\n");
		k_msleep(1000);
	}
	return 0;
}
