#include "GameRole.h"
#include"msg.pb.h"
#include"GameChannel.h"
#include"GameProtocol.h"
#include<algorithm>
#include<random>
#include<hiredis/hiredis.h>

static default_random_engine random_engine(time(NULL));
static AOIWorld aoiWorld(0,400,0,400,20,20);
static unsigned long cnt = 1;
void GameRole::viewAppear(GameRole* _role)
{
    //���෢��200����Ϣ
    auto msg = this->CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*msg, *(_role->m_protocol));

    msg = _role->CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*msg, *(this->m_protocol));

}
GameMsg* GameRole::CreateSelfPostion()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    pMsg->set_pid(id);
    pMsg->set_username(name);
    pMsg->set_tp(2);

    auto pPosition = pMsg->mutable_p();
    pPosition->set_x(x);
    pPosition->set_y(y);
    pPosition->set_z(z);
    pPosition->set_v(v);

    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pret;
}
void GameRole::viewLost(GameRole* _role)
{
    //���෢��201����Ϣ
    auto msg = this->getIDNameLogOff();
    ZinxKernel::Zinx_SendOut(*msg, *(_role->m_protocol));

    msg = _role->getIDNameLogOff();
    ZinxKernel::Zinx_SendOut(*msg, *(this->m_protocol));
}



GameMsg* GameRole::getIDNameLogin()
{
    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(id);
    pMsg->set_username(name);
    GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pMsg);
    return msg;
}

GameMsg* GameRole::getIDNameLogOff()
{
    pb::SyncPid* pMsg = new pb::SyncPid();
    pMsg->set_pid(id);
    pMsg->set_username(name);
    GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pMsg);
    return msg;
}

GameMsg* GameRole::getAroundPlayers()
{
    pb::SyncPlayers* pMsgs = new pb::SyncPlayers();
    auto playersList = aoiWorld.GetSrdPlayers(this);
    for (auto player : playersList) {
        auto pPlayer = pMsgs->add_ps(); //����pb::playerָ�룬Ȼ������ָ���������
        auto pRole = dynamic_cast<GameRole*>(player);
        pPlayer->set_pid(pRole->id);
        pPlayer->set_username(pRole->name);
        auto pPosition = pPlayer->mutable_p(); 
        pPosition->set_x(pRole->x);
        pPosition->set_y(pRole->y);
        pPosition->set_z(pRole->z);
        pPosition->set_v(pRole->v);
    }
    GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_SRD_POSTION, pMsgs);
    return msg;
}

GameMsg* GameRole::getSelfPosition()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    pMsg->set_pid(id);
    pMsg->set_username(name);
    pMsg->set_tp(2); //��ʾ������Ϣ�ǳ�ʼλ��
    auto pPosition = pMsg->mutable_p();
    pPosition->set_x(x);
    pPosition->set_y(y);
    pPosition->set_z(z);
    pPosition->set_v(v);

    GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_SRD_POSTION, pMsg);
    return msg;
}

GameMsg* GameRole::getBoardCastContent(std::string content)
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    pMsg->set_pid(id);
    pMsg->set_username(name);
    pMsg->set_tp(1);
    pMsg->set_content(content);
    GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return msg;
}

GameRole::GameRole()
{
    name = "mc" + std::to_string(cnt); cnt++;
    x = random_engine() % 50 +100;
    z = random_engine() % 50 +100;
    //�����ִ���redis���ݿ� ��ΪgameName������
    redisContext* rc = redisConnect("127.0.0.1", 6379);
    if (rc) {
        freeReplyObject(redisCommand(rc, "lpush gameName %s", name.c_str()));
        redisFree(rc);
    }
}

bool GameRole::Init()
{
    if (ZinxKernel::Zinx_GetAllRole().size() >= 0) {
        TimerOutMng::GetInstance().DelTask(ExitTimer::getInstance());
    }
    bool flag = aoiWorld.AddPlayer(this);
    /*�������idΪTCP����id�����ļ�������*/
    id = m_protocol->m_channel->GetFd();
    
    if (flag) {
        //����id������
        auto msgT = getIDNameLogin();
        ZinxKernel::Zinx_SendOut(*msgT, *m_protocol);
        //���Լ�������Χ��ҵ�λ��
        msgT = getAroundPlayers();
        ZinxKernel::Zinx_SendOut(*msgT, *m_protocol);
        //����Χ��ҷ����Լ���λ��
        auto srdPlayersList = aoiWorld.GetSrdPlayers(this);
        for (auto player : srdPlayersList) {
            auto pPlayer = dynamic_cast<GameRole*>(player);
            auto pMsg = CreateSelfPostion();
            ZinxKernel::Zinx_SendOut(*pMsg, *(pPlayer->m_protocol));
        }
        
    }
    return flag;
}

/*�����û�����*/
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    GET_REF2DATA(MultiMsgs, _input, _poUserData);
    for (auto signal : _input.msgs) {
        std::cout << signal->m_msg_type << " " << signal->pMsg->Utf8DebugString() << std::endl;
        //����Ⱥ��������Ϣ
        if (signal->m_msg_type == GameMsg::MSG_TYPE_CHAT_CONTENT) {
            //����Ϣ���͸������û�
            //ȡ����Ϣ����
            auto content = dynamic_cast<pb::Talk*>(signal->pMsg)->content();
            auto roles = ZinxKernel::Zinx_GetAllRole();
            for (auto role : roles) {
                auto pRole = dynamic_cast<GameRole*>(role);
                GameMsg* msg = getBoardCastContent(content);
                ZinxKernel::Zinx_SendOut(*msg, *(pRole->m_protocol));
            }
        }
        else if (signal->m_msg_type == GameMsg::MSG_TYPE_NEW_POSTION) {
            /*�ƶ��󣬸�����Ϣ*/
            //1.�ж��ǲ��ǿ�Խ����
            //��ȡ��ʼ��Χ����б���Ϊs1
            auto s1 = aoiWorld.GetSrdPlayers(this);
            //����λ�ú󣬻�ȡ�µ�����б���Ϊs2
            //ȡ���µ�λ��
            auto new_position = dynamic_cast<pb::Position*>(signal->pMsg);
            //�����Լ���λ��
            this->x = new_position->x();
            this->y = new_position->y();
            this->z = new_position->z();
            this->v = new_position->v();
            auto s2 = aoiWorld.GetSrdPlayers(this);
            //����s1  s1�д��ڣ�s2�в����ڣ���Ҫ����˫������Ұ����
            for (auto s1P : s1) {
                if (find(s2.begin(), s2.end(), s1P) == s2.end()) {
                    viewLost(dynamic_cast<GameRole*>(s1P));
                }
            }
            //����s2  s2�д��ڣ�s1�в����ڣ���Ҫ����˫������Ұappear
            for (auto s2P : s2) {
                if (find(s1.begin(), s1.end(), s2P) == s1.end()) {
                    viewAppear(dynamic_cast<GameRole*>(s2P));
                }
            }
            //2.����Χ������Ҹ���λ����Ϣ
            //���±����λ�ã�����λ�����GameMsg������Χ��ҷ���λ����Ϣ
            pb::BroadCast* pMsg = new pb::BroadCast();
            pMsg->set_pid(id);
            pMsg->set_username(name);
            pMsg->set_tp(4);
            auto position = pMsg->mutable_p();
            position->set_x(new_position->x());
            position->set_y(new_position->y());
            position->set_z(new_position->z());
            position->set_v(new_position->v());
            GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
            auto srdPlayersList = aoiWorld.GetSrdPlayers(this);
            for (auto player : srdPlayersList) {
                auto pPlayer = dynamic_cast<GameRole*>(player);
                ZinxKernel::Zinx_SendOut(*msg, *(pPlayer->m_protocol));
            }

        }

    }
    return nullptr;
}

void GameRole::Fini()
{
    //����Χ��ҷ����Լ����ߵ���Ϣ
    auto pMsg = getIDNameLogOff();
    auto srdPlayersList = aoiWorld.GetSrdPlayers(this);
    for (auto player : srdPlayersList) {
        auto pPlayer = dynamic_cast<GameRole*>(player);
        ZinxKernel::Zinx_SendOut(*pMsg, *(pPlayer->m_protocol));
    }
    aoiWorld.DelPlayer(this);

    /*��ֻʣ���һ����ң�������ҲҪ��ɾ��ʱ --������ʱ5�룬�رշ����*/
    if (ZinxKernel::Zinx_GetAllRole().size() <= 1) {
        //������ʱ����
        TimerOutMng::GetInstance().AddTask(ExitTimer::getInstance());
    }
    redisContext* rc = redisConnect("127.0.0.1", 6379);
    if (rc) {
        freeReplyObject(redisCommand(rc, "lrem gameName 0 %s", name.c_str()));
        redisFree(rc);
    }
}

int GameRole::GetX()
{
    return int(x);
}

int GameRole::GetY()
{
    return int(z);
}

void ExitTimer::Proc()
{
    ZinxKernel::Zinx_Exit();
}

int ExitTimer::GetTimeSec()
{
    return 5;
}
