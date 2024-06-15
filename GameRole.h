#pragma once
#include<zinx.h>
#include"GameMsg.h"
#include"AOIWorld.h"
#include"ZinxTimer.h"
class GameProtocol;//就用一下指针，不访问成员，能不包含头文件就别包含头文件，防止循环引用
class GameRole :
    public Irole, public Player
{
    float x = 0;
    float y = 0;//高
    float z = 0;
    float v = 0;//视角
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
    // 通过 Irole 继承
    virtual bool Init() override;
    virtual UserData* ProcMsg(UserData& _poUserData) override;
    virtual void Fini() override;

    // 通过 Player 继承
    virtual int GetX() override;
    virtual int GetY() override;
};

//定时关闭的任务
class ExitTimer : public TimerOutProc {
public:
    //设置单例
    static ExitTimer* getInstance() {
        static ExitTimer* exitTimer = new ExitTimer();
        return exitTimer;
    }
    // 通过 TimerOutProc 继承
    virtual void Proc() override;
    virtual int GetTimeSec() override;
};