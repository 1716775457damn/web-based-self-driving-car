// #include <Arduino.h>
// //本示例程序使用Stepper库
// #include <Stepper.h>

// // 定义电机控制用常量

// // 电机内部输出轴旋转一周步数
// const int STEPS_PER_ROTOR_REV = 32;

// //  减速比
// const int GEAR_REDUCTION = 64;

// /*
//  * 转子旋转一周需要走32步。转子每旋转一周，电机输出轴只旋转1/64周。
//  * （电机内部配有多个减速齿轮,这些齿轮会的作用是让转子每旋转一周，
//  * 输出轴只旋转1/64周。）
//  * 因此电机输出轴旋转一周则需要转子走32X64=2048步，即以下常量定义。
// */

// // 电机外部输出轴旋转一周步数 （2048）
// const float STEPS_PER_OUT_REV = STEPS_PER_ROTOR_REV * GEAR_REDUCTION;

// // 定义电机控制用变量

// // 电机旋转步数
// int StepsRequired;

// // 建立步进电机对象
// // 定义电机控制引脚以及电机基本信息。
// // 电机控制引脚为 8,9,10,11
// // 以上引脚依次连接在ULN2003 驱动板 In1, In2, In3, In4

// Stepper steppermotor(STEPS_PER_ROTOR_REV, 5, 18, 19, 21);

// void setup()
// {
// //setup函数内无内容
// }

// void loop()
// {
//   // 极慢转动4步用于观察ULN2003电机驱动板LED变化
//   steppermotor.setSpeed(1);
//   StepsRequired  =  4;
//   steppermotor.step(StepsRequired);
//   delay(1000);

//    // 慢速顺时针旋转一圈
//   StepsRequired  =  STEPS_PER_OUT_REV;
//   steppermotor.setSpeed(500);
//   steppermotor.step(StepsRequired);
//   delay(1000);

//   // 快速逆时针旋转一圈
//   StepsRequired  =  - STEPS_PER_OUT_REV;
//   steppermotor.setSpeed(800);
//   steppermotor.step(StepsRequired);
//   delay(2000);

// }

// #include<Arduino.h>
// #include <AccelStepper.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>

// // 定义步进电机的引脚
// #define STEPPER1_PIN1 5
// #define STEPPER1_PIN2 18
// #define STEPPER1_PIN3 19
// #define STEPPER1_PIN4 21

// #define STEPPER2_PIN1 9
// #define STEPPER2_PIN2 8
// #define STEPPER2_PIN3 7
// #define STEPPER2_PIN4 6

// // 定义继电器的引脚
// #define RELAY_PIN_1 10
// #define RELAY_PIN_2 11

//     // 创建步进电机对象
//     AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_PIN1, STEPPER1_PIN2, STEPPER1_PIN3, STEPPER1_PIN4);
// AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_PIN1, STEPPER2_PIN2, STEPPER2_PIN3, STEPPER2_PIN4);

// // 步进电机任务函数
// void stepperTask(void *pvParameters)
// {
//   AccelStepper *stepper = static_cast<AccelStepper *>(pvParameters);

//   for (;;)
//   {
//     stepper->run();
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//   }
// }

// // 继电器任务函数
// void relayTask(void *pvParameters)
// {
//   const int relayPin = static_cast<int>(reinterpret_cast<intptr_t>(pvParameters));

//   for (;;)
//   {
//     digitalWrite(relayPin, HIGH);          // 打开继电器
//     vTaskDelay(1000 / portTICK_PERIOD_MS); // 持续1秒
//     digitalWrite(relayPin, LOW);           // 关闭继电器
//     vTaskDelay(1000 / portTICK_PERIOD_MS); // 持续1秒
//   }
// }

// void setup()
// {
//   // 初始化步进电机
//   stepper1.setMaxSpeed(1000);
//   stepper1.setAcceleration(500);
//   stepper1.moveTo(5000);

//   stepper2.setMaxSpeed(1000);
//   stepper2.setAcceleration(500);
//   stepper2.moveTo(-5000);

//   // 初始化继电器引脚
//   pinMode(RELAY_PIN_1, OUTPUT);
//   pinMode(RELAY_PIN_2, OUTPUT);

//   // 创建步进电机任务
//   xTaskCreatePinnedToCore(stepperTask, "Stepper1", 2048, &stepper1, 1, NULL, tskNO_AFFINITY);
//   xTaskCreatePinnedToCore(stepperTask, "Stepper2", 2048, &stepper2, 1, NULL, tskNO_AFFINITY);

//   // 创建继电器任务
//   xTaskCreatePinnedToCore(relayTask, "Relay1", 1024, (void *)(intptr_t)RELAY_PIN_1, 2, NULL, tskNO_AFFINITY);
//   xTaskCreatePinnedToCore(relayTask, "Relay2", 1024, (void *)(intptr_t)RELAY_PIN_2, 2, NULL, tskNO_AFFINITY);
// }

// void loop()
// {
//   // 主循环为空，任务由FreeRTOS管理
// }

#include <Arduino.h>
#include <Stepper.h>

// 电机内部输出轴旋转一周步数
const int STEPS_PER_ROTOR_REV = 32;

// 减速比
const int GEAR_REDUCTION = 64;

// 电机外部输出轴旋转一周步数 （2048）
const float STEPS_PER_OUT_REV = STEPS_PER_ROTOR_REV * GEAR_REDUCTION;

// 电机控制对象
Stepper steppermotor(STEPS_PER_ROTOR_REV, 5, 18, 19, 21);

void setup()
{
    // 设置电机速度，使其旋转100圈每分钟
    float stepsPerSecond = 20 * STEPS_PER_OUT_REV / 60.0;
    steppermotor.setSpeed(stepsPerSecond);
}

void loop()
{
    // 旋转100圈每分钟
    int stepsToRotate = STEPS_PER_OUT_REV;
    steppermotor.step(stepsToRotate);

    // 等待一秒钟，然后继续下一轮旋转
    delay(10);
}