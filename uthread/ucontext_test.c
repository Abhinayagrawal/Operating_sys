#include<stdio.h>
#include<ucontext.h>

int main(){
	ucontext_t cont1, cont2;
	getcontext(&cont1);
	printf("Hello world");
	setcontext(&cont1);
	return 0;
}
