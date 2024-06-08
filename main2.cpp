#include <iostream>
#include <cstring>  // 包含cstring头文件以使用strncpy和strlen
#include <cmqc.h>   // 包含IBM MQ C API头文件
#include <cmqxc.h>  // 包含扩展的IBM MQ头文件

void checkMQICompletion(MQLONG completionCode, MQLONG reasonCode, const char* errorMessage) {
    if (completionCode != MQCC_OK) {
        std::cerr << errorMessage << " Reason code: " << reasonCode << std::endl;
        exit(1);
    }
}

#define BUFFER_LENGTH 1024  // 自定义缓冲区长度

int main() {
    MQHCONN hConn;  // MQ连接句柄
    MQHOBJ hObj;    // MQ对象句柄
    MQOD od = {MQOD_DEFAULT};  // 对象描述符
    MQMD md = {MQMD_DEFAULT};  // 消息描述符
    MQGMO gmo = {MQGMO_DEFAULT};  // 获取消息选项
    MQLONG completionCode;  // 完成代码
    MQLONG reasonCode;  // 原因代码
    MQCNO cno = {MQCNO_DEFAULT};  // 连接选项
    MQCD cd = {MQCD_CLIENT_CONN_DEFAULT};  // 通道定义

    // 设置连接参数
    strncpy(cd.ChannelName, "YOUR_CHANNEL_NAME", MQ_CHANNEL_NAME_LENGTH);
    strncpy(cd.ConnectionName, "YOUR_HOSTNAME(PORT)", MQ_CONN_NAME_LENGTH);

    // 连接选项中指定通道定义
    cno.ClientConnPtr = &cd;

    // 连接到队列管理器
    MQCONNX((MQCHAR*)"YOUR_QMGR_NAME", &cno, &hConn, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "Failed to connect to queue manager");

    // 设置队列名称
    strncpy(od.ObjectName, "YOUR_QUEUE_NAME", MQ_Q_NAME_LENGTH);

    // 打开队列
    MQOPEN(hConn, &od, MQOO_INPUT_AS_Q_DEF, &hObj, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "Failed to open queue");

    // 准备获取消息
    MQBYTE buffer[BUFFER_LENGTH];
    MQLONG bufferLength = BUFFER_LENGTH;
    MQLONG messageLength;

    // 获取消息
    gmo.Options = MQGMO_WAIT | MQGMO_FAIL_IF_QUIESCING;
    gmo.WaitInterval = 5000;  // 设置等待时间（毫秒）

    MQGET(hConn, hObj, &md, &gmo, bufferLength, buffer, &messageLength, &completionCode, &reasonCode);
    if (completionCode == MQCC_OK) {
        std::cout << "Received message: " << std::string(reinterpret_cast<char*>(buffer), messageLength) << std::endl;
    } else if (completionCode == MQCC_WARNING && reasonCode == MQRC_NO_MSG_AVAILABLE) {
        std::cerr << "No message available in the queue." << std::endl;
    } else {
        checkMQICompletion(completionCode, reasonCode, "Failed to get message from queue");
    }

    // 关闭队列
    MQCLOSE(hConn, &hObj, MQCO_NONE, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "Failed to close queue");

    // 断开连接
    MQDISC(&hConn, &completionCode, &reasonCode);
    checkMQICompletion(completionCode, reasonCode, "Failed to disconnect from queue manager");

    return 0;
}