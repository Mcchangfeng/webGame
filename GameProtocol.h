#pragma once
#include<zinx.h>
#include<iostream>
#include<string>
#include"GameMsg.h"
using namespace std;

class GameChannel;
class GameRole;
class GameProtocol :
    public Iprotocol
{
public:
    std::string lastStr;
    GameChannel* m_channel = nullptr;
    GameRole* m_role = nullptr;
    ~GameProtocol();
    // Í¨¹ý Iprotocol ¼Ì³Ð
    virtual UserData* raw2request(std::string _szInput) override;
    virtual std::string* response2raw(UserData& _oUserData) override;
    virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};

