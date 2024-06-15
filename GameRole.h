#pragma once
#include<zinx.h>
#include"GameMsg.h"
#include"AOIWorld.h"
#include"ZinxTimer.h"
class GameProtocol;//����һ��ָ�룬�����ʳ�Ա���ܲ�����ͷ�ļ��ͱ����ͷ�ļ�����ֹѭ������
class GameRole :
    public Irole, public Player
{
    float x = 0;
    float y = 0;//��
    float z = 0;
    float v = 0;//�ӽ�
    int id = 0;
    std::string name = "";
    void viewAppear(GameRole* _role);
    void viewLost(GameRole* _role);
    GameMsg* getIDNameLogin();
    GameMsg* getIDNameLogOff();
    GameMsg* getAroundPlayers();
    GameMsg* getSelfPosition();
    GameMsg* getBoardCastContent(std::string content);
    GameMsg* CreateSelfPostion();
public:
    GameRole();
    GameProtocol* m_protocol = nullptr;
    // ͨ�� Irole �̳�
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;

    // ͨ�� Player �̳�
    virtual int GetX() override;
    virtual int GetY() override;
};

//��ʱ�رյ�����
class ExitTimer : public TimerOutProc {
public:
    //���õ���
    static ExitTimer* getInstance() {
        static ExitTimer* exitTimer = new ExitTimer();
        return exitTimer;
    }
    // ͨ�� TimerOutProc �̳�
    virtual void Proc() override;
    virtual int GetTimeSec() override;
};