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
    //创建TCP通道、协议和业务对象
    GameChannel* channel = new GameChannel(_fd);
    GameProtocol* prot = new GameProtocol();
    GameRole* role = new GameRole();
    //通道与协议互相绑定
    channel->m_prot = prot;
    prot->m_channel = channel;
    //协议与业务互相绑定
    prot->m_role = role;
    role->m_protocol = prot;
    //将协议和业务对象添加到kernel
    ZinxKernel::Zinx_Add_Proto(*prot);
    ZinxKernel::Zinx_Add_Role(*role);
    return channel;
}
