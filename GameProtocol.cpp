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
        /*��ǰ�ĸ��ֽڶ�ȡ��Ϣ����*/
        int msgLen = 0;
        memcpy(&msgLen, lastStr.c_str(), sizeof(int));
        // msgLen = ntohl(msgLen); //�ֽ���ת��
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
* ������ҵ��㷢��������
* ����ֵ�Ƿ��ظ�ͨ�����������
*/
//std::string* GameProtocol::response2raw(UserData& _oUserData)
//{
//    // �����ص��ַ���
//    std::string tStr = "";
//    // �м����
//    int msgLen = 0;
//    int id = 0;
//    string msgStr = "";
//    GET_REF2DATA(GameMsg, _output, _oUserData);
//    id = _output.m_msg_type;
//    msgStr = _output.serialize();
//    msgLen = msgStr.length();
//    // �����ȱ�Ϊ�ַ���
//    uint8_t str[4];
//    memcpy(&str, &msgLen, sizeof(int));
//    tStr = tStr + string(reinterpret_cast<char*>(str), sizeof(str));
//    //��id��Ϊ�ַ���
//    memcpy(&str, &id, sizeof(int));
//    tStr = tStr + string(reinterpret_cast<char*>(str), sizeof(str));
//    //��������Ϣ��
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
