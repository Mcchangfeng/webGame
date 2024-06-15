#include<iostream>
#include"GameChannel.h"
#include<ZinxTCP.h>
#include<zinx.h>
#include"msg.pb.h"
#include"GameMsg.h"
#include"ZinxTimer.h"


//void daemonlize() {
//	/*设置守护进程*/
//	//1. fork
//	int pid = fork();
//	if (pid < 0) {
//		exit(-1);
//	}
//	if (pid > 0) {
//		//2. 父进程退出
//		exit(0);
//	}
//	//3. 子进程设置会话ID
//	setsid();
//	//4. 子进程设置执行路径 不设置默认为当前路径
//	//5. 重定向标准输入、输出、错误，这三个文件描述符到/dev/null
//	int nullId = open("/dev/null",O_RDWR);
//	if (nullId > 0) {
//		dup2(nullId, 0);
//		dup2(nullId, 1);
//		dup2(nullId, 2);
//		close(nullId);
//	}
//	/*设置监控进程，如果程序异常退出，重新启动服务*/
//	while (1) {
//		int pidC = fork();
//		if (pidC < 0) {
//			exit(-1);
//		}
//		if (pidC > 0) { //监控进程
//			int cStatus = 0;
//			wait(&cStatus);
//			if (cStatus == 0) { //退出码为0，表示正常退出；那就直接退出即可
//				exit(0);
//			}
//		}
//		else { //业务进程
//			break;
//		}
//	}
//}
void daemonlize()
{
	//1 fork
	int ipid = fork();
	if (0 > ipid)
	{
		exit(-1);
	}
	if (0 < ipid)
	{
		//2 父进程退出
		exit(0);
	}

	//3 子进程 设置回话ID
	setsid();
	//4 子进程 设置执行路径

	//5 子进程 重定向3个文件描述到/dev/null
	int nullfd = open("/dev/null", O_RDWR);
	if (nullfd >= 0)
	{
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}

	//进程监控
	while (1)
	{
		int pid = fork();
		if (0 > pid)
		{
			exit(-1);
		}

		/*父进程等子进程退出*/
		if (0 < pid)
		{
			int iStatus = 0;
			wait(&iStatus);
			if (0 == iStatus)
			{
				exit(0);
			}
		}
		/*子进程跳出循环执行游戏业务*/
		else
		{
			break;
		}
	}
}
int main() {
	daemonlize();
	ZinxKernel::ZinxKernelInit();
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(51112,new GameTcpFact())));
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));
	ZinxKernel::Zinx_Run();
	 
	ZinxKernel::ZinxKernelFini();

	return 0;
}