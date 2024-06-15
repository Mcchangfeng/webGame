#include<iostream>
#include"GameChannel.h"
#include<ZinxTCP.h>
#include<zinx.h>
#include"msg.pb.h"
#include"GameMsg.h"
#include"ZinxTimer.h"


//void daemonlize() {
//	/*�����ػ�����*/
//	//1. fork
//	int pid = fork();
//	if (pid < 0) {
//		exit(-1);
//	}
//	if (pid > 0) {
//		//2. �������˳�
//		exit(0);
//	}
//	//3. �ӽ������ûỰID
//	setsid();
//	//4. �ӽ�������ִ��·�� ������Ĭ��Ϊ��ǰ·��
//	//5. �ض����׼���롢����������������ļ���������/dev/null
//	int nullId = open("/dev/null",O_RDWR);
//	if (nullId > 0) {
//		dup2(nullId, 0);
//		dup2(nullId, 1);
//		dup2(nullId, 2);
//		close(nullId);
//	}
//	/*���ü�ؽ��̣���������쳣�˳���������������*/
//	while (1) {
//		int pidC = fork();
//		if (pidC < 0) {
//			exit(-1);
//		}
//		if (pidC > 0) { //��ؽ���
//			int cStatus = 0;
//			wait(&cStatus);
//			if (cStatus == 0) { //�˳���Ϊ0����ʾ�����˳����Ǿ�ֱ���˳�����
//				exit(0);
//			}
//		}
//		else { //ҵ�����
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
		//2 �������˳�
		exit(0);
	}

	//3 �ӽ��� ���ûػ�ID
	setsid();
	//4 �ӽ��� ����ִ��·��

	//5 �ӽ��� �ض���3���ļ�������/dev/null
	int nullfd = open("/dev/null", O_RDWR);
	if (nullfd >= 0)
	{
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}

	//���̼��
	while (1)
	{
		int pid = fork();
		if (0 > pid)
		{
			exit(-1);
		}

		/*�����̵��ӽ����˳�*/
		if (0 < pid)
		{
			int iStatus = 0;
			wait(&iStatus);
			if (0 == iStatus)
			{
				exit(0);
			}
		}
		/*�ӽ�������ѭ��ִ����Ϸҵ��*/
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