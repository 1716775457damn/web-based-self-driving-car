
#include <Arduino.h>
#include <WiFi.h>
#include <WiFi.h>
#define TCP_SERVER_ADDR "bemfa.com"
#define TCP_SERVER_PORT "8344"
#ifndef AP_CONFIG
String UID = "c5af7df6bf9558c03f69df6ca3b3db12";
#endif
#define MAX_PACKETSIZE 512
#define KEEPALIVEATIME 30 * 1000
WiFiClient TCPclient;
String TcpClient_Buff = "";
unsigned int TcpClient_BuffIndex = 0;
unsigned long TcpClient_preTick = 0;
unsigned long preHeartTick = 0;
unsigned long preTCPStartTick = 0;
bool preTCPConnected = false;
bool subFlag = false;

// 定义连接到L298N的引脚
const int motorAin1 = 17; // 电机A的输入1
const int motorAin2 = 16; // 电机A的输入2
const int motorBin1 = 5;  // 电机B的输入1
const int motorBin2 = 18; // 电机B的输入2
const int enA = 19;       // 电机A的使能引脚
const int enB = 21;       // 电机B的使能引脚
char c, w, b, l, r, s;
// 初始化电机控制函数
void setupMotorPins()
{
    pinMode(motorAin1, OUTPUT);
    pinMode(motorAin2, OUTPUT);
    pinMode(motorBin1, OUTPUT);
    pinMode(motorBin2, OUTPUT);
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);

    digitalWrite(enA, LOW); // 默认使ENA处于启用状态
    digitalWrite(enB, LOW); // 默认使ENB处于启用状态
}

// 函数声明
void setMotorSpeed(int motorPin1, int motorPin2, int speed);
void stopMotor(int motorPin1, int motorPin2);
void enableMotor(int enPin);
void disableMotor(int enPin);
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();

// setup 和 loop 函数保持不变

// 启用电机
void enableMotor(int enPin)
{
    digitalWrite(enPin, HIGH);
}

// 禁用电机
void disableMotor(int enPin)
{
    digitalWrite(enPin, LOW);
}

// 设置电机速度（正数向前，负数向后）
void setMotorSpeed(int motorPin1, int motorPin2, int speed)
{
    if (speed > 0)
    {
        digitalWrite(motorPin1, HIGH);
        digitalWrite(motorPin2, LOW);
    }
    else if (speed < 0)
    {
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, HIGH);
    }
    else
    {
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
    }
    analogWrite(enA, abs(speed));
    analogWrite(enB, abs(speed));
}

// 停止电机
void stopMotor(int motorPin1, int motorPin2)
{
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    analogWrite(enA, 0);
    analogWrite(enB, 0);
}

// 小车前进
void moveForward()
{
    setMotorSpeed(motorAin1, motorAin2, 150); // 电机A向前
    setMotorSpeed(motorBin1, motorBin2, 150); // 电机B向前
}

// 小车后退
void moveBackward()
{
    setMotorSpeed(motorAin1, motorAin2, -150); // 电机A向后
    setMotorSpeed(motorBin1, motorBin2, -150); // 电机B向后
}

// 小车左转
void turnLeft()
{
    setMotorSpeed(motorAin1, motorAin2, -150); // 电机A向后
    setMotorSpeed(motorBin1, motorBin2, 150);  // 电机B向前
}

// 小车右转
void turnRight()
{
    setMotorSpeed(motorAin1, motorAin2, 150);  // 电机A向前
    setMotorSpeed(motorBin1, motorBin2, -150); // 电机B向后
}

void doWiFiTick();
void doTCPClientTick();
void sendtoTCPServer(String p);
void publishMsg(String topic, String msg)
{
    String tcpTemp = "";
    tcpTemp = "cmd=2&uid=" + UID + "&topic=" + topic + "&msg=" + msg + "\r\n";
    sendtoTCPServer(tcpTemp);
    tcpTemp = "";
    Serial.println("A message has been published");
}
void subTopic(String topic)
{
    String tcpTemp = "";
    tcpTemp = "cmd=1&uid=" + UID + "&topic=" + topic + "\r\n";
    sendtoTCPServer(tcpTemp);
    tcpTemp = "";
    Serial.print("sub topic:");
    Serial.println(topic);
}
void startTCPClient()
{
    if (TCPclient.connect(TCP_SERVER_ADDR, atoi(TCP_SERVER_PORT)))
    {
        Serial.print("Connected to server:");
        Serial.println(TCP_SERVER_ADDR);
        subFlag = true;
        preHeartTick = millis();
        TCPclient.setNoDelay(true);
    }
    else
    {
        Serial.print("Failed connected to server:");
        Serial.println(TCP_SERVER_ADDR);
        TCPclient.stop();
        preTCPConnected = false;
    }
    preTCPStartTick = millis();
}

void doWiFiTick()
{
    static bool taskStarted = false;
    static uint32_t lastWiFiCheckTick = 0;
    if (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - lastWiFiCheckTick > 1000)
        {
            lastWiFiCheckTick = millis();
            WiFi.begin();
        }
    }
    else
    {
        if (taskStarted == false)
        {
            taskStarted = true;
            Serial.print("Get IP Address: ");
            Serial.println(WiFi.localIP());
            startTCPClient();
        }
    }
}
void sendtoTCPServer(String p)
{
    if (!TCPclient.connected())
    {
        Serial.println("Client is not readly");
        return;
    }
    TCPclient.print(p);
    preHeartTick = millis(); // 心跳计时开始，需要每隔60秒发送一次数据
}

void subTopicCenter()
{
    if (subFlag == true)
    {
        subFlag = false;

        subTopic("mycar");
    }
}

void doTCPClientTick()
{
    if (WiFi.status() != WL_CONNECTED)
        return;
    if (!TCPclient.connected())
    {
        if (preTCPConnected == true)
        {
            preTCPConnected = false;
            preTCPStartTick = millis();
            Serial.println();
            Serial.println("TCP Client disconnected.");
            TCPclient.stop();
        }
        else if (millis() - preTCPStartTick > 1 * 1000)
            startTCPClient();
    }
    else
    {
        if (TCPclient.available())
        {
            char c = TCPclient.read();
            TcpClient_Buff += c;
            TcpClient_BuffIndex++;
            TcpClient_preTick = millis();
            if (TcpClient_BuffIndex >= MAX_PACKETSIZE - 1)
            {
                TcpClient_BuffIndex = MAX_PACKETSIZE - 2;
                TcpClient_preTick = TcpClient_preTick - 200;
            }
        }
        if (millis() - preHeartTick >= KEEPALIVEATIME)
        {
            preHeartTick = millis();
            Serial.println("--Keep alive:");
            sendtoTCPServer("ping\r\n");
        }
    }
    if ((TcpClient_Buff.length() >= 1) && (millis() - TcpClient_preTick >= 200))
    {
        TCPclient.flush();
        TcpClient_Buff.trim();
        String topic = "";
        String msg = "";
        if (TcpClient_Buff.length() > 15)
        {
            int topicIndex = TcpClient_Buff.indexOf("&topic=") + 7;
            int msgIndex = TcpClient_Buff.indexOf("&msg=");
            topic = TcpClient_Buff.substring(topicIndex, msgIndex);
            msg = TcpClient_Buff.substring(msgIndex + 5);
        }

        if (msg == "w")
        {
            moveForward();
            delay(1000);
            publishMsg("mycar", "s");
        }
        if (msg == "b")
        {
            moveBackward();
            delay(1000);
            publishMsg("mycar", "s");
        }
        if (msg == "l")
        {
            turnLeft();
            delay(1000);
            publishMsg("mycar", "s");
        }
        if (msg == "r")
        {
            turnRight();
            delay(1000);
            publishMsg("mycar", "s");
        }
        if (msg == "s")
        {
            stopMotor(motorAin1, motorAin2);
            stopMotor(motorBin1, motorBin2);
        }
        msg = "";
        topic = "";
        TcpClient_Buff = "";
        TcpClient_BuffIndex = 0;
    }
}

void setup()
{ // 初始化引脚为输出模式
    setupMotorPins();

    // 启动时停止所有电机
    stopMotor(motorAin1, motorAin2);
    stopMotor(motorBin1, motorBin2);

    Serial.begin(9600);
    WiFi.begin("ovo", "twx20051");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Local IP:");
    Serial.print(WiFi.localIP());
}

void loop()
{
    doWiFiTick();
    doTCPClientTick();
    subTopicCenter();
}
