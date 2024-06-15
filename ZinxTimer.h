#pragma once
#include <zinx.h>
#include <vector>
#include <list>

class ZinxTimerChannel :
	public Ichannel
{
	int m_TimerFd = -1;
public:
	ZinxTimerChannel();
	virtual ~ZinxTimerChannel();

	// ͨ�� Ichannel �̳�
	virtual bool Init() override;

	virtual bool ReadFd(std::string & _input) override;
	virtual bool WriteFd(std::string & _output) override;
	virtual void Fini() override;
	virtual int GetFd() override;
	virtual std::string GetChannelInfo() override;
	virtual AZinxHandler * GetInputNextStage(BytesMsg & _oInput) override;
};

class TimerOutProc {
public:
	virtual void Proc() = 0;
	virtual int GetTimeSec() = 0;
	/*��ʣȦ��*/
	int iCount = -1;
};

class TimerOutMng :public AZinxHandler {
	std::vector<std::list<TimerOutProc *> > m_timer_wheel;
	int cur_index = 0;
	static TimerOutMng single;
	TimerOutMng();
public:
	/*����ʱ�¼�,�������г�ʱ����*/
	virtual IZinxMsg * InternelHandle(IZinxMsg & _oInput) override;

	virtual AZinxHandler * GetNextHandler(IZinxMsg & _oNextMsg) override;
	void AddTask(TimerOutProc *_ptask);
	void DelTask(TimerOutProc *_ptask);
	static TimerOutMng &GetInstance() {
		return single;
	}

};

