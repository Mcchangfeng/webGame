#include "GameChannel.h"

GameChannel::GameChannel(int _fd) : ZinxTcpData(_fd)
{
}

GameChannel::~GameChannel() {
    if (m_prot != nullptr) {
        ZinxKernel::Zinx_Del_Proto(*m_prot);
        delete m_prot;
    }
}

AZinxHandler* GameChannel::GetInputNextStage(BytesMsg& _oInput)
{
    return m_prot;
}

ZinxTcpData* GameTcpFact::CreateTcpDataChannel(int _fd)
{
    //����TCPͨ����Э���ҵ�����
    GameChannel* channel = new GameChannel(_fd);
    GameProtocol* prot = new GameProtocol();
    GameRole* role = new GameRole();
    //ͨ����Э�黥���
    channel->m_prot = prot;
    prot->m_channel = channel;
    //Э����ҵ�����
    prot->m_role = role;
    role->m_protocol = prot;
    //��Э���ҵ�������ӵ�kernel
    ZinxKernel::Zinx_Add_Proto(*prot);
    ZinxKernel::Zinx_Add_Role(*role);
    return channel;
}
