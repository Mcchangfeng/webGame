# webGame
## Introtuction
本项目使用ZINX框架，搭建了一套游戏后端服务器程序，实现了多人在线网游。服务程序采用三层框架设计，第一层为通道层，第二层为协议层，第三层为业务层。通道层继承ZINX框架的TCP类，负责建立TCP连接、通信等，实现与客户端的数据发送与接收。协议层负责将将通道层的数据，转化为自定义的用户数据，交给业务层处理；也可以完成将业务层的数据转化为字符串，交给通道层，进行发送。业务层负责处理业务数据，项目中设计了群发消息，同步移动数据等业务。
