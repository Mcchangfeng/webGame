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
    //互相发送200号消息
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
    //互相发送201号消息
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
        auto pPlayer = pMsgs->add_ps(); //返回pb::player指针，然后对这个指针进行设置
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
    pMsg->set_tp(2); //表示这条消息是初始位置
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
    //将名字存入redis数据库 键为gameName的链表
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
    /*设置玩家id为TCP连接id，即文件描述符*/
    id = m_protocol->m_channel->GetFd();
    
    if (flag) {
        //发送id和姓名
        auto msgT = getIDNameLogin();
        ZinxKernel::Zinx_SendOut(*msgT, *m_protocol);
        //向自己发送周围玩家的位置
        msgT = getAroundPlayers();
        ZinxKernel::Zinx_SendOut(*msgT, *m_protocol);
        //向周围玩家发送自己的位置
        auto srdPlayersList = aoiWorld.GetSrdPlayers(this);
        for (auto player : srdPlayersList) {
            auto pPlayer = dynamic_cast<GameRole*>(player);
            auto pMsg = CreateSelfPostion();
            ZinxKernel::Zinx_SendOut(*pMsg, *(pPlayer->m_protocol));
        }
        
    }
    return flag;
}

/*处理用户请求*/
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    GET_REF2DATA(MultiMsgs, _input, _poUserData);
    for (auto signal : _input.msgs) {
        std::cout << signal->m_msg_type << " " << signal->pMsg->Utf8DebugString() << std::endl;
        //处理群发聊天消息
        if (signal->m_msg_type == GameMsg::MSG_TYPE_CHAT_CONTENT) {
            //将消息发送给所有用户
            //取出消息内容
            auto content = dynamic_cast<pb::Talk*>(signal->pMsg)->content();
            auto roles = ZinxKernel::Zinx_GetAllRole();
            for (auto role : roles) {
                auto pRole = dynamic_cast<GameRole*>(role);
                GameMsg* msg = getBoardCastContent(content);
                ZinxKernel::Zinx_SendOut(*msg, *(pRole->m_protocol));
            }
        }
        else if (signal->m_msg_type == GameMsg::MSG_TYPE_NEW_POSTION) {
            /*移动后，更新信息*/
            //1.判断是不是跨越网格
            //获取初始周围玩家列表，记为s1
            auto s1 = aoiWorld.GetSrdPlayers(this);
            //更新位置后，获取新的玩家列表，记为s2
            //取出新的位置
            auto new_position = dynamic_cast<pb::Position*>(signal->pMsg);
            //更新自己的位置
            this->x = new_position->x();
            this->y = new_position->y();
            this->z = new_position->z();
            this->v = new_position->v();
            auto s2 = aoiWorld.GetSrdPlayers(this);
            //遍历s1  s1中存在，s2中不存在，需要进行双方的视野消除
            for (auto s1P : s1) {
                if (find(s2.begin(), s2.end(), s1P) == s2.end()) {
                    viewLost(dynamic_cast<GameRole*>(s1P));
                }
            }
            //遍历s2  s2中存在，s1中不存在，需要进行双方的视野appear
            for (auto s2P : s2) {
                if (find(s1.begin(), s1.end(), s2P) == s1.end()) {
                    viewAppear(dynamic_cast<GameRole*>(s2P));
                }
            }
            //2.向周围所有玩家更新位置信息
            //更新本玩家位置，将新位置组成GameMsg，向周围玩家发送位置信息
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
    //向周围玩家发送自己下线的消息
    auto pMsg = getIDNameLogOff();
    auto srdPlayersList = aoiWorld.GetSrdPlayers(this);
    for (auto player : srdPlayersList) {
        auto pPlayer = dynamic_cast<GameRole*>(player);
        ZinxKernel::Zinx_SendOut(*pMsg, *(pPlayer->m_protocol));
    }
    aoiWorld.DelPlayer(this);

    /*当只剩最后一个玩家，且马上也要被删除时 --》倒计时5秒，关闭服务端*/
    if (ZinxKernel::Zinx_GetAllRole().size() <= 1) {
        //启动定时任务
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
