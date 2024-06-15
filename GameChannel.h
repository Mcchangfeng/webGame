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
    // 通过 ZinxTcpData 继承
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class GameTcpFact :public IZinxTcpConnFact {
    // 通过 IZinxTcpConnFact 继承
    virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};

