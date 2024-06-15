#include "GameMsg.h"

GameMsg::GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg):m_msg_type(_type),pMsg(_pMsg)
{
}

GameMsg::GameMsg(MSG_TYPE _type, std::string _str):m_msg_type(_type)
{
	switch (_type)
	{
	case GameMsg::MSG_TYPE_LOGIN_ID_NAME:
		pMsg = new pb::SyncPid();
		break;
	case GameMsg::MSG_TYPE_CHAT_CONTENT:
		pMsg = new pb::Talk();
		break;
	case GameMsg::MSG_TYPE_NEW_POSTION:
		pMsg = new pb::Position();
		break;
	case GameMsg::MSG_TYPE_BROADCAST:
		pMsg = new pb::BroadCast();
		break;
	case GameMsg::MSG_TYPE_LOGOFF_ID_NAME:
		pMsg = new pb::SyncPid();
		break;
	case GameMsg::MSG_TYPE_SRD_POSTION:
		pMsg = new pb::Position();
		break;
	default:
		break;
	}
	/*将参数解析成系列化对象*/
	pMsg->ParseFromString(_str);
}

std::string GameMsg::serialize()
{
	std::string str;
	pMsg->SerializeToString(&str);
	return str;
}

GameMsg::~GameMsg()
{
	if (pMsg != nullptr) {
		delete pMsg;
	}
}
