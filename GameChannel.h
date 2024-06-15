#include<ZinxTCP.h>
#include"GameProtocol.h"
#include"GameRole.h"
class GameChannel :
    public ZinxTcpData
{
public:
    GameProtocol* m_prot = nullptr;
    GameChannel(int _fd);
    ~GameChannel();
    // ͨ�� ZinxTcpData �̳�
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class GameTcpFact :public IZinxTcpConnFact {
    // ͨ�� IZinxTcpConnFact �̳�
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};

