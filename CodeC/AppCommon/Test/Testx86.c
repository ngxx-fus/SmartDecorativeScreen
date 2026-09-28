#include <stdio.h>

#define SYS_LOG_ENGINE SerialPrint
#include "../AppCommon.h"

void SerPrint(SSize_t Size, const char * Buff){
	printf("%s", Buff);
}

Error_t main(){
	SysInfo("Hello!");
	return 0;
}