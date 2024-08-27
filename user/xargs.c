#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void 
// 执行Command对应的命令行，Argv是要传入的参数数组
execCommand(char* Command, char* Argv[MAXARG])
{
    if(fork() == 0)
        exec(Command, Argv);
    else
        wait(0);
    
}

int 
main(int argc, char* argv[])
{
    char CmdPath[512] = "", *Ptr;
    // 参数数组
    char* Argv[MAXARG];                                    
    int StartIndex = argc - 1;                              

    /* E.g: xargs grep hello */
    // 首先解析出原有的命令
    Ptr = CmdPath + strlen(CmdPath);
    memmove(Ptr, argv[1],  strlen(argv[1]));
    Ptr[strlen(argv[1])] = 0;                               
	
	// 将原有指令后面的参数记录下来，argv[0]是xargs，故从1开始
    for(int i = 1 ; i < argc ; ++i)                         
    {
        uint Length = strlen(argv[i]) + 1;
        Argv[i - 1] = malloc(Length); 
        memmove(Argv[i - 1], argv[i], Length); 
    }
	// 设置Argv[Argc] = NULL
    Argv[StartIndex + 1] = 0;                               
	
	// 因为是通过|读入的其他参数，所以这里从标准输入一个个字符读入即可
	// 遇到换行符说明这个额外参数已经结束
    char ReadChar;
    Argv[StartIndex] = malloc(512);                         
    Ptr = Argv[StartIndex];                                 
    while(read(0, &ReadChar, 1))                            
    {
        if(ReadChar != '\n')
        {
            *Ptr = ReadChar;
            ++Ptr;
        }
        else
        {
        	// 一旦遇到结尾，给字符串末尾一个空字符，执行此命令行即可
            *Ptr = 0;                                       
            execCommand(CmdPath, Argv);     
            Ptr = Argv[StartIndex];                         
        }
    }
	
	// 最后释放内存
    for(int i = 0 ; i <= StartIndex ; ++i)                  
        free(Argv[i]);

    exit(0);
}
