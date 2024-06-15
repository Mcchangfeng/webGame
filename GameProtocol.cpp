#include "GameProtocol.h"
#include"GameChannel.h"
#include"msg.pb.h"
GameProtocol::~GameProtocol()
{
    if (m_role != nullptr) {
        ZinxKernel::Zinx_Del_Role(*m_role);
        delete m_role;
    }
}
UserData* GameProtocol::raw2request(std::string _szInput)
{
    lastStr += _szInput;
    MultiMsgs* multiMsgs = new MultiMsgs();
    while (1) {
        if (lastStr.length() < 8) {
            break;
        }
        /*在前四个字节读取消息长度*/
        int msgLen = 0;
        memcpy(&msgLen, lastStr.c_str(), sizeof(int));
        // msgLen = ntohl(msgLen); //字节序转换
        int _id = 0;
        memcpy(&_id, lastStr.substr(4, 4).c_str(), sizeof(int));
        if (msgLen > lastStr.length() - 8) {
            break;
        }
        string msgComand = lastStr.substr(8, msgLen);
        GameMsg* pMsg = new GameMsg((GameMsg::MSG_TYPE)(_id), msgComand);
        multiMsgs->msgs.push_back(pMsg);
        lastStr.erase(0, 8 + msgLen);
    }

    return multiMsgs;
}

/*
* 参数是业务层发来的数据
* 返回值是返回给通道输出的数据
*/
//std::string* GameProtocol::response2raw(UserData& _oUserData)
//{
//    // 待返回的字符串
//    std::string tStr = "";
//    // 中间变量
//    int msgLen = 0;
//    int id = 0;
//    string msgStr = "";
//    GET_REF2DATA(GameMsg, _output, _oUserData);
//    id = _output.m_msg_type;
//    msgStr = _output.serialize();
//    msgLen = msgStr.length();
//    // 将长度变为字符串
//    uint8_t str[4];
//    memcpy(&str, &msgLen, sizeof(int));
//    tStr = tStr + string(reinterpret_cast<char*>(str), sizeof(str));
//    //将id变为字符串
//    memcpy(&str, &id, sizeof(int));
//    tStr = tStr + string(reinterpret_cast<char*>(str), sizeof(str));
//    //最后加上消息体
//    tStr += msgStr;
//    return &tStr;
//}
std::string* GameProtocol::response2raw(UserData& _oUserData)
{
    int iLength = 0;
    int id = 0;
    std::string MsgContent;

    GET_REF2DATA(GameMsg, oOutput, _oUserData);
    id = oOutput.m_msg_type;
    MsgContent = oOutput.serialize();
    iLength = MsgContent.size();

    auto pret = new std::string();

    pret->push_back((iLength >> 0) & 0xff);
    pret->push_back((iLength >> 8) & 0xff);
    pret->push_back((iLength >> 16) & 0xff);
    pret->push_back((iLength >> 24) & 0xff);
    pret->push_back((id >> 0) & 0xff);
    pret->push_back((id >> 8) & 0xff);
    pret->push_back((id >> 16) & 0xff);
    pret->push_back((id >> 24) & 0xff);
    pret->append(MsgContent);

    return pret;
}
Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    return m_role;
}

Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
    return m_channel;
}
