#include "includes.h"
/*****************函数声明*************/
/*<<<<<<< HEAD:Unicorn-V1.0/User/main.c*/
static void vTaskFLASH(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);
/*****************变量声明**************/
static TaskHandle_t xHandleTaskFLASH = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
/**
  * @brief  主循环
  * @param  None
  * @retval None
  */
int main(void)
{         
          LCD_Init();
          KEY_Init();
          LED_Init();
          Beep_Init();
          LIGHT_Init();										//初始化
         //__set_PRIMASK(1); 
        //初始化 RTU模式 从机地址为1 USART1 9600 无校验
          eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE);
        //启动FreeModbus 
          eMBEnable();
        // 创建任务
        AppTaskCreate();
                
        /* 启动调度，开始执行任务 */
        vTaskStartScheduler();
        /* 
        如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或者空闲任务的
        heap空间不足造成创建失败，此要加大FreeRTOSConfig.h文件中定义的heap大小：
        #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
        */
          while(1);
}
/*
*********************************************************************************************************
*	函 数 名: vTaskFLASH
*	功能说明: 接口消息处理，这里用作FLASH
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 1  (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
//要写入到STM32 FLASH的字符串数组
//#define FLASH_SAVE_ADDR  0X08020000   
//#define SIZE sizeof(FreeSize)	 	//数组长度
//u8 FreeSize[]={"STM32 ModbusTEST"};  
//u8 datatemp[SIZE];   
bool GO_1=false;
extern u8 menu;
bool flag_LED=false,flag_Beep=false;
//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
static void vTaskFLASH(void *pvParameters)
{
    while(1)
    {
         KEY_red();
        if(GO_1==true)	
        {
          OLED_function(menu);
	}
        if(flag_LED==true)
        {
           LCD_CLS();
           OLED_P6x8Str(0,4,"LED !"); 
           Led1_Set(LED_ON);vTaskDelay(500);Led1_Set(LED_OFF);		//点亮LED4，并延时500ms，然后熄灭LED4
               
           Led2_Set(LED_ON);vTaskDelay(500);Led2_Set(LED_OFF);		//点亮LED5，并延时500ms，然后熄灭LED5
                      
           Led3_Set(LED_ON);vTaskDelay(500);Led3_Set(LED_OFF);		//点亮LED6，并延时500ms，然后熄灭LED6
                      
           Led4_Set(LED_ON);vTaskDelay(500);Led4_Set(LED_OFF);		//点亮LED7，并延时500ms，然后熄灭LED7
           flag_LED=false;
        }
        if(flag_Beep==true)
        {
             LCD_CLS();
             OLED_P6x8Str(0,4,"Beep !"); 
             Beep_Set(BEEP_ON);
             vTaskDelay(1000);
             Beep_Set(BEEP_OFF);           //打开蜂鸣器,关闭蜂鸣器
             flag_Beep=false;
       }
       //STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);
      //STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)FreeSize,SIZE);
      vTaskDelay(100);
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTaskLED
*	功能说明: LED闪烁	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 2  
*********************************************************************************************************
*/
static void vTaskLED(void *pvParameters)
{
    /* LED闪烁 */
    while(1)
    {
      LIGHT_GetVoltag() ;//光敏
      vTaskDelay(100);
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTaskMsgPro
*	功能说明: 信息处理，这里是用作LED闪烁	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 3  
*********************************************************************************************************
*/
static void vTaskMsgPro(void *pvParameters)
{
    while(1)
    { 
        eMBPoll(); 
        vTaskDelay(300);
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTaskStart
*	功能说明: 启动任务，也就是最高优先级任务，这里用作LED闪烁
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
    while(1)
    {
        LED_Poll();
        vTaskDelay(400);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    xTaskCreate( vTaskFLASH,   	/* 任务函数  */
                 "vTaskFLASH",     	/* 任务名    */
                 512,               	/* 任务栈大小，单位word，也就是4字节 */
                 NULL,              	/* 任务参数  */
                 1,                 	/* 任务优先级*/
                 &xHandleTaskFLASH );  /* 任务句柄  */
	
	
	xTaskCreate( vTaskLED,    		/* 任务函数  */
                 "vTaskLED",  		/* 任务名    */
                 512,         		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 2,           		/* 任务优先级*/
                 &xHandleTaskLED ); /* 任务句柄  */
	
	xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务名    */
                 512,             		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 3,               		/* 任务优先级*/
                 &xHandleTaskMsgPro );  /* 任务句柄  */
	
	
	xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务名    */
                 512,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 4,              		/* 任务优先级*/
                 &xHandleTaskStart );   /* 任务句柄  */
}
//
//#include "includes.h"
///*****************鍑芥暟澹版槑*************/
//static void vTaskFLASH(void *pvParameters);
//static void vTaskLED(void *pvParameters);
//static void vTaskMsgPro(void *pvParameters);
//static void vTaskStart(void *pvParameters);
//static void AppTaskCreate (void);
///*****************鍙橀噺澹版槑**************/
//static TaskHandle_t xHandleTaskFLASH = NULL;
//static TaskHandle_t xHandleTaskLED = NULL;
//static TaskHandle_t xHandleTaskMsgPro = NULL;
//static TaskHandle_t xHandleTaskStart = NULL;
///**
//  * @brief  涓诲惊鐜�
//  * @param  None
//  * @retval None
//  */
//int main(void)
//{         
//          LCD_Init();
//          KEY_Init();
//          LED_Init();
//        //__set_PRIMASK(1); 
//        //鍒濆鍖� RTU妯″紡 浠庢満鍦板潃涓�1 USART1 9600 鏃犳牎楠�
//          eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE);
//        //鍚姩FreeModbus 
//          eMBEnable();
//        // 鍒涘缓浠诲姟
//        AppTaskCreate();
//                
//        /* 鍚姩璋冨害锛屽紑濮嬫墽琛屼换鍔� */
//        vTaskStartScheduler();
//        /* 
//        濡傛灉绯荤粺姝ｅ父鍚姩鏄笉浼氳繍琛屽埌杩欓噷鐨勶紝杩愯鍒拌繖閲屾瀬鏈夊彲鑳芥槸鐢ㄤ簬瀹氭椂鍣ㄤ换鍔℃垨鑰呯┖闂蹭换鍔＄殑
//        heap绌洪棿涓嶈冻閫犳垚鍒涘缓澶辫触锛屾瑕佸姞澶reeRTOSConfig.h鏂囦欢涓畾涔夌殑heap澶у皬锛�
//        #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
//        */
//                while(1);
//}
///*
//*********************************************************************************************************
//*	鍑� 鏁� 鍚�: vTaskFLASH
//*	鍔熻兘璇存槑: 鎺ュ彛娑堟伅澶勭悊锛岃繖閲岀敤浣淔LASH
//*	褰�    鍙�: pvParameters 鏄湪鍒涘缓璇ヤ换鍔℃椂浼犻�掔殑褰㈠弬
//*	杩� 鍥� 鍊�: 鏃�
//*   浼� 鍏� 绾�: 1  (鏁板�艰秺灏忎紭鍏堢骇瓒婁綆锛岃繖涓窡uCOS鐩稿弽)
//*********************************************************************************************************
//*/
////瑕佸啓鍏ュ埌STM32 FLASH鐨勫瓧绗︿覆鏁扮粍
//#define FLASH_SAVE_ADDR  0X08020000   
//#define SIZE sizeof(FreeSize)	 	//鏁扮粍闀垮害
//u8 FreeSize[]={"STM32 ModbusTEST"};  
//u8 datatemp[SIZE];   	
////璁剧疆FLASH 淇濆瓨鍦板潃(蹇呴』涓哄伓鏁帮紝涓斿叾鍊艰澶т簬鏈唬鐮佹墍鍗犵敤FLASH鐨勫ぇ灏�+0X08000000)
//static void vTaskFLASH(void *pvParameters)
//{
//   
//    while(1)
//    {
//       //STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);
//      //STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)FreeSize,SIZE);
//      vTaskDelay(100);
//    }
//}
//
///*
//*********************************************************************************************************
//*	鍑� 鏁� 鍚�: vTaskLED
//*	鍔熻兘璇存槑: LED闂儊	
//*	褰�    鍙�: pvParameters 鏄湪鍒涘缓璇ヤ换鍔℃椂浼犻�掔殑褰㈠弬
//*	杩� 鍥� 鍊�: 鏃�
//*   浼� 鍏� 绾�: 2  
//*********************************************************************************************************
//*/
//static void vTaskLED(void *pvParameters)
//{
//    /* LED闂儊 */
//    while(1)
//    {
//         LED0_ON();
//         vTaskDelay(100);
//         LED0_OFF();
//    }
//}
//
///*
//*********************************************************************************************************
//*	鍑� 鏁� 鍚�: vTaskMsgPro
//*	鍔熻兘璇存槑: 淇℃伅澶勭悊锛岃繖閲屾槸鐢ㄤ綔LED闂儊	
//*	褰�    鍙�: pvParameters 鏄湪鍒涘缓璇ヤ换鍔℃椂浼犻�掔殑褰㈠弬
//*	杩� 鍥� 鍊�: 鏃�
//*   浼� 鍏� 绾�: 3  
//*********************************************************************************************************
//*/
//static void vTaskMsgPro(void *pvParameters)
//{
//    while(1)
//    {
//        
//        eMBPoll(); 
//        vTaskDelay(300);
//    }
//}
//
///*
//*********************************************************************************************************
//*	鍑� 鏁� 鍚�: vTaskStart
//*	鍔熻兘璇存槑: 鍚姩浠诲姟锛屼篃灏辨槸鏈�楂樹紭鍏堢骇浠诲姟锛岃繖閲岀敤浣淟ED闂儊
//*	褰�    鍙�: pvParameters 鏄湪鍒涘缓璇ヤ换鍔℃椂浼犻�掔殑褰㈠弬
//*	杩� 鍥� 鍊�: 鏃�
//*   浼� 鍏� 绾�: 4  
//*********************************************************************************************************
//*/
//static void vTaskStart(void *pvParameters)
//{
//    while(1)
//    {
//        KEY_red();
//        LED_Poll();
//        vTaskDelay(400);
//    }
//}
//
///*
//*********************************************************************************************************
//*	鍑� 鏁� 鍚�: AppTaskCreate
//*	鍔熻兘璇存槑: 鍒涘缓搴旂敤浠诲姟
//*	褰�    鍙傦細鏃�
//*	杩� 鍥� 鍊�: 鏃�
//*********************************************************************************************************
//*/
//static void AppTaskCreate (void)
//{
//    xTaskCreate( vTaskFLASH,   	/* 浠诲姟鍑芥暟  */
//                 "vTaskFLASH",     	/* 浠诲姟鍚�    */
//                 512,               	/* 浠诲姟鏍堝ぇ灏忥紝鍗曚綅word锛屼篃灏辨槸4瀛楄妭 */
//                 NULL,              	/* 浠诲姟鍙傛暟  */
//                 1,                 	/* 浠诲姟浼樺厛绾�*/
//                 &xHandleTaskFLASH );  /* 浠诲姟鍙ユ焺  */
//	
//	
//	xTaskCreate( vTaskLED,    		/* 浠诲姟鍑芥暟  */
//                 "vTaskLED",  		/* 浠诲姟鍚�    */
//                 512,         		/* 浠诲姟鏍堝ぇ灏忥紝鍗曚綅word锛屼篃灏辨槸4瀛楄妭 */
//                 NULL,        		/* 浠诲姟鍙傛暟  */
//                 2,           		/* 浠诲姟浼樺厛绾�*/
//                 &xHandleTaskLED ); /* 浠诲姟鍙ユ焺  */
//	
//	xTaskCreate( vTaskMsgPro,     		/* 浠诲姟鍑芥暟  */
//                 "vTaskMsgPro",   		/* 浠诲姟鍚�    */
//                 512,             		/* 浠诲姟鏍堝ぇ灏忥紝鍗曚綅word锛屼篃灏辨槸4瀛楄妭 */
//                 NULL,           		/* 浠诲姟鍙傛暟  */
//                 3,               		/* 浠诲姟浼樺厛绾�*/
//                 &xHandleTaskMsgPro );  /* 浠诲姟鍙ユ焺  */
//	
//	
//	xTaskCreate( vTaskStart,     		/* 浠诲姟鍑芥暟  */
//                 "vTaskStart",   		/* 浠诲姟鍚�    */
//                 512,            		/* 浠诲姟鏍堝ぇ灏忥紝鍗曚綅word锛屼篃灏辨槸4瀛楄妭 */
//                 NULL,           		/* 浠诲姟鍙傛暟  */
//                 4,              		/* 浠诲姟浼樺厛绾�*/
//                 &xHandleTaskStart );   /* 浠诲姟鍙ユ焺  */
//}
//>>>>>>> 9fd0c1753aa48fb6da98c1c2c579a419d710233d:FreeModbus+FreeRTOS-8.0/User/main.c
