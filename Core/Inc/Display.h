#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"


// Per definire quanto tempo aspettare da quando un pulsante � stato premuto
uint32_t MAX_DELAY = 0x2710; // 10000 in decimale =	10 secondi

// define per i comandi
#define WRITEOBJ 0x01
#define WRITESTR 0x02

// define per gli oggetti
#define FORM 0x0A
#define USERLED 0x13
#define LENGTH_OBJ 6

// define per gli indici
#define SCREEN0 0x00	// per MAIN
#define SCREEN1 0x01	// per RTD

#define SCREEN2 0x02
#define SCREEN3 0x03
#define SCREEN4 0x04
#define SCREEN5 0x05
#define SCREEN6 0x06
#define SCREEN7 0x07

// msb ed lsb per cambiare form sono sempre 0
#define MSB_FORM0 0x00
#define LSB_FORM0 0x00



//Indirizzi CAN ricezione/Invio
#define Steering_START 0x60
#define Steering_MAIN 0x61
#define BMSLV_VBattery 0x
#define BMSHV_VCell 0x206
#define BMSHV_VBattery 0x204

// UART variables

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

// Variabili per mandare il comando al display
extern uint8_t length;
extern uint8_t index_d;
extern uint8_t cmd;
extern uint8_t id;
extern uint8_t msb;
extern uint8_t lsb;
extern uint8_t checksum_obj;
extern uint8_t checksum;

// Variabili per cambiare le informazioni sulle schermate
extern uint8_t HV;
extern uint8_t temp_HV;
extern uint8_t soc;
extern uint8_t temp_mot_f;
extern uint8_t temp_mot_r;
extern uint8_t temp_inv;
extern uint8_t LV;
extern uint8_t temp_LV;
extern uint8_t speed;
extern uint8_t brake;
extern uint8_t err_bms;
extern uint8_t err_inv;
extern uint8_t steering;
extern uint8_t HV_VCell_Max;
extern uint8_t HV_VCell_Min;
extern uint8_t LV_VBattery;
extern uint8_t HV_VBattery;

extern uint8_t counter;



bool WriteObject (uint8_t cmd, uint8_t ID, uint8_t index, uint8_t msb, uint8_t lsb, uint8_t checksum);
bool WriteStr (uint8_t cmd, uint8_t index, uint8_t length, uint8_t message[], uint8_t checksum);
uint8_t Dec_To_Hex_Conversion(long int decimalNumber);
void initialization();
void screen(uint8_t RTD, uint8_t message[], size_t len, uint32_t delay_100us);
void Change_Objects_Values();
void Core_Steering_Wheel();
uint8_t checksum_objects(uint8_t cmd, uint8_t id, uint8_t index, uint8_t msb, uint8_t lsb);
int delay_fun (uint32_t *delay_100us_last, uint32_t delay_100us);
uint32_t ReturnTime_100us(void);
int return_len_string(uint8_t message[]);
void switch_screen(uint32_t delay_100us, uint8_t message[], size_t len);
void check_push_dx(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t delay_100us, uint8_t screen);
void check_push_sx(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t delay_100us, uint8_t screen);

//funzioni CAN
void CAN_Tx(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void CAN_Msg_Send(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox, uint32_t TimeOut);