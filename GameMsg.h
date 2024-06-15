#pragma once
#include<zinx.h>
#include<google/protobuf/message.h>
#include"msg.pb.h"
#include<list>
class GameMsg :
    public UserData
{
public:
    google::protobuf::Message* pMsg = nullptr;
	enum MSG_TYPE
	{
		MSG_TYPE_LOGIN_ID_NAME = 1, 
		MSG_TYPE_CHAT_CONTENT = 2, 
		MSG_TYPE_NEW_POSTION = 3, 
		MSG_TYPE_BROADCAST = 200,
		MSG_TYPE_LOGOFF_ID_NAME = 201, 
		MSG_TYPE_SRD_POSTION = 202
	}m_msg_type;
	/*构造函数*/ 
	GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg);
	GameMsg(MSG_TYPE _type, std::string _str);

	/*序列化*/
	std::string serialize();

	virtual ~GameMsg();
};

class MultiMsgs :public UserData {
public:
	std::list<GameMsg*> msgs;
};