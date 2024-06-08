#include <iostream>
#include <cstring>  // 包含cstring头文件以使用strncpy和strlen
#include <cmqc.h>   // 包含IBM MQ C API头文件
#include <cmqxc.h>  // 包含扩展的IBM MQ头文件
#include <thread>

void checkMQICompletion(MQLONG completionCode, MQLONG reasonCode, const char* operation) {
    if (completionCode != MQCC_OK) {
        std::cerr << operation << " failed , Reason code: " << reasonCode << std::endl;
        //exit(1);
        return;
    }

    std::cout << operation << " succeed" << std::endl;
}

#define BUFFER_LENGTH 1024  // 自定义缓冲区长度

int main() {
    MQHCONN hConn;  // MQ连接句柄
    MQHOBJ hObj;    // MQ对象句柄
    MQOD od = {MQOD_DEFAULT};  // 对象描述符
    MQMD md = {MQMD_DEFAULT};  // 消息描述符
    MQPMO pmo = {MQPMO_DEFAULT};  // 发布消息选项
    MQLONG completionCode;  // 完成代码
    MQLONG reasonCode;  // 原因代码
    MQCNO cno = {MQCNO_DEFAULT};  // 连接选项
    MQCD cd = {MQCD_CLIENT_CONN_DEFAULT};  // 通道定义

    // 设置连接参数
    strncpy(cd.ChannelName, "USER.QM2.TESTCHANNEL", MQ_CHANNEL_NAME_LENGTH);
    strncpy(cd.ConnectionName, "192.168.72.135(1515)", MQ_CONN_NAME_LENGTH);

    // 连接选项中指定通道定义
    cno.ClientConnPtr = &cd;

    // 连接到队列管理器
    MQCONNX((MQCHAR*)"QM2", &cno, &hConn, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "connect to queue manager");
    if (completionCode != MQCC_OK) {
        exit(1);
    }

    // 设置队列名称
    strncpy(od.ObjectName, "TEST_QUEUE", MQ_Q_NAME_LENGTH);

    // 打开队列
    MQOPEN(hConn, &od, MQOO_OUTPUT, &hObj, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "open queue");
    if (completionCode != MQCC_OK) {
        exit(1);
    }

    // 准备消息内容
    const char* message = "Hello, IBM MQ!";
    MQBYTE buffer[BUFFER_LENGTH];
    strncpy(reinterpret_cast<char*>(buffer), message, BUFFER_LENGTH);

    // 发布消息
    MQLONG messageLength = strlen(message);
    MQPUT(hConn, hObj, &md, &pmo, messageLength, buffer, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "put message to queue");
    if (completionCode == MQCC_OK) {
        std::cout << "Message successfully put to queue" << std::endl;
    }

    //std::this_thread::sleep_for(std::chrono::seconds(100));
    // 关闭队列
    MQCLOSE(hConn, &hObj, MQCO_NONE, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "close queue");

    // 断开连接
    MQDISC(&hConn, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "disconnect from queue manager");

    return 0;
}
