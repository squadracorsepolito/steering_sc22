#include "Display.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
uint8_t length;
uint8_t index;
uint8_t HV;
uint8_t temp_HV;
uint8_t soc;
uint8_t temp_mot_f;
uint8_t temp_mot_r;
uint8_t temp_inv;
uint8_t LV;
uint8_t temp_LV;
uint8_t speed;
uint8_t brake;
uint8_t err_bms;
uint8_t err_inv;
uint8_t steering;
uint8_t id;
uint8_t msb;
uint8_t lsb;
uint8_t checksum_obj;
uint8_t checksum;


uint8_t counter;

int page=0;

void initialization(){
	HV = 0x00;
	temp_HV = 0x00;
	soc = 0x00;
	temp_mot_f = 0x00;
	temp_mot_r = 0x00;
	temp_inv = 0x00;
	LV = 0x00;
	temp_LV = 0x00;
	speed = 0x00;
	brake = 0x00;
	err_bms = 0x00;
	err_inv = 0x00;
	steering = 0x00;
	id = 0x00;
	msb = 0x00;
	lsb = 0x00;
	checksum_obj = 0x00;
	
}


// calcolo la lunghezza della stringa e me la ritorno
int return_len_string(uint8_t message[]){
	
	int len;
	len = sizeof(message)/sizeof(message[0]);
	return len;
}


/*
Per calcolare il checksum, devo effettuare lo xor di coppie di bytes, per cui, se ad esempio ho "0a24", dovrò fare lo xor tra 0a e 24
per cui 00001010 xor 00100100 = 00101110 = 2e
*/
uint8_t checksum_objects(uint8_t cmd, uint8_t id, uint8_t index, uint8_t msb, uint8_t lsb){
	
	/*
	unsigned int sum;
	for (sum = 0; len!= 0; len --){
		
		sum += *(message++);
		
	}return(sum);
	*/
	
	uint8_t a;
	uint8_t b;
	uint8_t c;
	
	/*	QUESTO PER LE STRINGHE
	int n = 0;
	len = return_len_string(message);
	
	for (int i=0; i< len; i++){
		
			if (i < len-1) {
				
				checksum = message[i] ^ message[i+1];
			}
		}

*/

	a = cmd ^ id;
	b = index ^ msb;
	c = a ^ b;
	checksum_obj = c ^ lsb;

	return checksum_obj;
	
}


/* La funzione WriteObject mi serve per "pilotare" un oggetto VisiGenie.
	Essa prende come parametri di ingresso il comando (vedi datasheet VisiGenie sul drive), l'ID dell'oggetto (sempre sul datasheet),
	l'indice dell'oggetto (che vedo da workshop, cliccando sull'oggetto), il msb e l'lsb che assumono il valore che voglio assumano 
	(in base al valore che voglio loro assumano) e infine il checksum, che calcolo online.
	
	Tutti questi parametri di ingresso li utilizzo come elementi di un vettore puntato (*) castato poi a (uint8_t []) per mandarlo dopo
	in UART tramite una UART_Transmit in modo da mandare tutto il comando di scrittura dell'oggetto.
	
	Successivamente ricevo il byte di acknowledgement che il display manda alla scheda e lo mando in un if in cui controllo se stia trasmettendo bene
	(e quindi ricevo 0x06) oppure se stia trasmettendo male (quindi ricevo 0x15). In base a questo, ritorno 0 o 1.

*/
bool WriteObject (uint8_t cmd, uint8_t ID, uint8_t index, uint8_t msb, uint8_t lsb, uint8_t checksum){
	
	initialization();
	
	cmd = WRITEOBJ;	
	uint8_t ACK[1];
	uint8_t *command = (uint8_t []){cmd, ID, index, msb, lsb, checksum};
	
	HAL_UART_Transmit(&huart1, (uint8_t*)command, LENGTH_OBJ, 100);
	HAL_UART_Receive(&huart1, (uint8_t*)ACK, 1, 100);
	
	
	if (*ACK == 0x06){
		return true;
	}
	if (*ACK == 0x15) {
		return false;
	}
}



// Funzione per scrivere valori negli oggetti di tipo stringa

bool WriteStr (uint8_t cmd, uint8_t index, uint8_t length, uint8_t message[], uint8_t checksum){
	
	cmd = WRITESTR;
	uint8_t ACK[1];
	
	// Con le stringhe, trasmetto una variabile alla volta, perchè altrimenti non funziona con la trasmissione del messaggio di tipo stringa
	
	//HAL_UART_Transmit(&huart1, (uint8_t*)com, length+4, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)&cmd, 1, 100);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t*)&index, 1, 100);
	HAL_Delay(1);
	HAL_UART_Transmit(&huart1, (uint8_t*)&length, 1, 100);
	HAL_Delay(1);
	for (int i=0; i<length; i++){
	HAL_UART_Transmit(&huart1, &message[i], 1, 100);	// ciclo su tutta la lunghezza della stringa, così da inviare un valore per volta
	HAL_Delay(1);
	}
	
	HAL_UART_Transmit(&huart1, (uint8_t*)&checksum, 1, 100);
	HAL_Delay(1);
	
	//HAL_Delay(500);
	HAL_UART_Receive(&huart1, (uint8_t*)ACK, 1, 100);
	//HAL_Delay(500);
	
	
	if (*ACK == 0x06){
		return true;
	}
	if (*ACK == 0x15) {
		return false;
	}
}




// Funzione che mi permette di capire su quale schermata deve stare fisso il display
// Questo dipende dal valore della variabile RTD che ci viene mandata via CAN da dSpace

void screen(uint8_t RTD, uint8_t message[], size_t len, uint32_t delay_100us) {
	
	uint32_t delay_100us_last;
	
	switch (RTD){
		
		case 0: 
		
			// PER CAMBIARE I VALORI DI UNA STRINGA, LA LUNGHEZZA DEVO CALCOLARLA FUORI DALLA FUNZIONE WriteStr, ALTRIMENTI DA' PROBLEMI.
			// GUARDARE MAIN CODICE STRINGFUNCTIONS PER AVER PRESENTE COME FARE !!! 
			len = return_len_string(message);
			checksum = checksum_objects(WRITEOBJ, FORM, SCREEN0, MSB_FORM0, LSB_FORM0);
			WriteObject(WRITEOBJ, FORM, SCREEN0, MSB_FORM0, LSB_FORM0, checksum);	// 0x0B è il checksum per la schermata main
			delay_fun(&delay_100us_last, delay_100us);	// piccolissimo ritardo per iniziare a cambiare i valori sul display
		
			//SD = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI SD
			// err_bms = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// HV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// err_inv =FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// soc = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// brake = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// steering = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// LV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
		break;
		
		case 1:
			
			len = return_len_string(message);
			checksum = checksum_objects(WRITEOBJ, FORM, SCREEN0, MSB_FORM0, LSB_FORM0);
			WriteObject(WRITEOBJ, FORM, SCREEN0, MSB_FORM0, LSB_FORM0, checksum);
			delay_fun(&delay_100us_last, delay_100us);	// piccolissimo ritardo per iniziare a cambiare i valori sul display
			//SD = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI SD
			// err_bms = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// HV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// err_inv =FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// soc = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// brake = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// steering = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// LV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS

		break;
		
		case 2:
			
		len = return_len_string(message);
		checksum = checksum_objects(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0);
		WriteObject(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0, checksum);	// 0x0A è il checksum per la schermata RTD
		delay_fun(&delay_100us_last, delay_100us);	// piccolissimo ritardo per iniziare a cambiare i valori sul display
		
			// HV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI SD
			// temp_HV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// temp_mot_f = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// temp_mot_r =FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// soc = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// temp_inv = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// temp_LV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// LV = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
			// speed = FUNZIONE CHE RESTITUISCE IN CAN IL VALORE DI ERR BMS
		
	}
}


void check_push_dx(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t delay_100us, uint8_t screen){
	
	bool first_push = 1;	//variabile che utilizzo per capire se abbia premuto il pulsante
	uint32_t waiting = 0;	// variabile per aspettare che il pulsante sia ancora attivo e quindi eseguo le mie operazioni
	uint32_t time = 0;		// variabile per contare quanto tempo sia passato da un push e verificare quindi che quel push sia ancora a 1
	
	
	// Check sul pulsante che incrementa il numero di pagine
	
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1)		{	// Push 1: incrementa
			// se ho premuto il pulsante per incrementare
			 if (first_push == 1) {				// se ho premuto il pulsante per la prima volta
				time = ReturnTime_100us();	// calcolo il tempo per metterlo nella delay_fun e fare il check poi sul tempo trascorso
				first_push  = 0;						// metto a zero il push
				waiting = 1;								// entro in attesa per fare il secondo check
			}
		}
		if (waiting){
			if (delay_fun(&time, delay_100us))	// se è passato un tempo maggiore di time	
					{
						if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1){		// ri-checko che il pulsante sia ancora pushato
							
							page++;			// incremento per passare alla pagina successiva
							checksum = checksum_objects(WRITEOBJ, FORM, screen, MSB_FORM0, LSB_FORM0);;	
							WriteObject(WRITEOBJ, FORM, screen, MSB_FORM0, LSB_FORM0, checksum);	// qui passo alla schermata successiva
						}			
						
							if (delay_fun(&MAX_DELAY, delay_100us) && HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)==0){	// se passano 10 secondi e non sto più premendo il pulsante, torno alla schermata RTD
							checksum = checksum_objects(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0);	
							WriteObject(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0, checksum);	// se dopo 10 secondi non premo più nulla, torno al form1 RTD
							page = 0;
							}
						
						waiting = 0;	// non sono più in attesa, quindi lo metto a zero
						first_push = 1;	// rimetto il push a 1, pronto per la prossima azione, quando ci sarà un altro pulsante da premere
					}
		}
		
}


void check_push_sx(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t delay_100us, uint8_t screen){
	
	bool first_push = 1;	//variabile che utilizzo per capire se abbia premuto il pulsante
	uint32_t waiting = 0;	// variabile per aspettare che il pulsante sia ancora attivo e quindi eseguo le mie operazioni
	uint32_t time = 0;		// variabile per contare quanto tempo sia passato da un push e verificare quindi che quel push sia ancora a 1
	
	
	// Check sul pulsante che incrementa il numero di pagine
	
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1)		{	// Push 1: incrementa
			// se ho premuto il pulsante per incrementare
			 if (first_push == 1) {				// se ho premuto il pulsante per la prima volta
				time = ReturnTime_100us();	// calcolo il tempo per metterlo nella delay_fun e fare il check poi sul tempo trascorso
				first_push  = 0;						// metto a zero il push
				waiting = 1;								// entro in attesa per fare il secondo check
			}
		}
		if (waiting){
			if (delay_fun(&time, delay_100us))	// se è passato un tempo maggiore di time	
					{
						if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1){		// ri-checko che il pulsante sia ancora pushato
							
							page--;			// decremento per passare alla pagina successiva
							checksum = checksum_objects(WRITEOBJ, FORM, screen, MSB_FORM0, LSB_FORM0);;	
							WriteObject(WRITEOBJ, FORM, screen, MSB_FORM0, LSB_FORM0, checksum);	// qui passo alla schermata precedente
						}
							if (delay_fun(&MAX_DELAY, delay_100us) && HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)==0){	// se passano 10 secondi e non sto premendo più il pulsante, torno alla schermata RTD
							checksum = checksum_objects(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0);	
							WriteObject(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0, checksum);	// se dopo 10 secondi non premo più nulla, torno al form1 RTD
							page = 0;
							}
						
						waiting = 0;	// non sono più in attesa, quindi lo metto a zero
						first_push = 1;	// rimetto il push a 1, pronto per la prossima azione, quando ci sarà un altro pulsante da premere
					}
		}
		
}

void switch_screen(uint32_t delay_100us, uint8_t message[], size_t len){
	
	bool first_push = 1;	//variabile che utilizzo per capire se abbia premuto il pulsante
	uint32_t waiting = 0;	// variabile per aspettare che il pulsante sia ancora attivo e quindi eseguo le mie operazioni
	uint32_t time = 0;		// variabile per contare quanto tempo sia passato da un push e verificare quindi che quel push sia ancora a 1
	
	
	len = return_len_string(message);
	
	switch(page){
		
		case 0:	// parto dalla schermata 1, quindi dalla schermata RTD
			
		checksum = checksum_objects(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0);
		WriteObject(WRITEOBJ, FORM, SCREEN1, MSB_FORM0, LSB_FORM0, checksum);	// parto dalla schermata RTD
			
		check_push_dx(GPIOF, GPIO_PIN_2, delay_100us, SCREEN2);
		check_push_sx(GPIOE, GPIO_PIN_4, delay_100us, SCREEN0);
		
		break;
		
		case 1:
				// Sono nella schermata 2
		//checksum = checksum_objects(WRITEOBJ, FORM, SCREEN2);
			
		break;
	
	}
}

// Funzione per capire dopo quanto tempo si va in overflow e quindi si scatena un interrupt. Questa funzione è utilizzata per la delay_fun
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	
	if ((*htim).Instance == TIM3 ) {
		counter++;
	}
}


uint32_t ReturnTime_100us(void) {

	uint32_t time = counter;	// counter è inizializzate a 0. Ad ogni interrupt che si verifica, poi, si incrementa perchè l'ARR è andato in overflow. 
						// Il contatore, infatti, è incrementato ogni 100 us
	return time;

		// return the value of the counter incremented every 100 us --> Lavora con counter che viene incrementato ad ogni overflow dell'ARR
	
}



int delay_fun (uint32_t *delay_100us_last, uint32_t delay_100us){
	
	// Return true if the delay is completed, false otherwise

	uint32_t current_time = ReturnTime_100us();	// sarebbe il valore di counter a quel momento

	if (current_time > *delay_100us_last && (current_time - *delay_100us_last) >= delay_100us) {

		*delay_100us_last = current_time;
		return 1;
	} else if (current_time < *delay_100us_last && (0xFFFFFFFF - current_time - *delay_100us_last) >= delay_100us) {

		*delay_100us_last = current_time;
		return 1;
	
		}
	}


void Core_SteeringWheel(){
	
	uint32_t delay_100_last, delay_100;
	
	initialization();
	// checksum = funzione per il calcolo del checksum
	WriteObject(cmd, FORM, index, MSB_FORM0, LSB_FORM0, checksum);
	delay_fun(&delay_100_last, delay_100);
	
	
}



uint8_t Dec_To_Hex_Conversion(long int decimalNumber){
	
	long int remainder,quotient;
	int i=1,j,temp;
	char hexadecimalNumber[100];
	quotient = decimalNumber;
	while(quotient!=0) {
		temp = quotient % 16;
		//To convert integer into character
		if( temp < 10)
		           temp =temp + 48; 
		else
		         temp = temp + 55;
		
		hexadecimalNumber[i++]= temp;
		quotient = quotient / 16;
	}
	
	
	return decimalNumber;
}



//CAN functions
void CAN_Msg_Send(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox, uint32_t TimeOut) {

	HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_SET);
	
	mailbox = HAL_CAN_GetTxMailboxesFreeLevel(hcan);
	
	if(mailbox > 0) {
		HAL_StatusTypeDef ret;

		ret = HAL_CAN_AddTxMessage(hcan, pHeader, aData, pTxMailbox);
			
		switch (ret) {
			case HAL_OK:
				++HAL_OK_COUNT;
				break;
			
			case HAL_ERROR:
				++HAL_ERROR_COUNT;
				break;

			case HAL_BUSY:
				++HAL_BUSY_COUNT; 
				break;
			
			case HAL_TIMEOUT:
				++HAL_TIMEOUT_COUNT;
				break;
		}	
		
		if (ret != HAL_OK) {
			//Error_Handler();
			HAL_CAN_ResetError(hcan);
			HAL_CAN_AbortTxRequest(hcan, *pTxMailbox);
		}
		
	} else {
		HAL_CAN_FULL++;
		
		HAL_CAN_ResetError(hcan);
		HAL_CAN_AbortTxRequest(hcan, *pTxMailbox);
	}
	
	HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_RESET);

}



void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {

	/* Transmission request Error */
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
		HAL_CAN_ResetError(hcan);
		Error_Handler();
  }
	
	
	switch(RxHeader.StdId) {
		
		case Steering_START:
		
			if( (RxHeader.DLC == 8)) ) {
				
			}
			break;
		
		case Steering_MAIN:
		
			if( (RxHeader.DLC == 8)) ) {
				
			}
			break;
			
		case BMSLV_VBattery:
			if( (RxHeader.DLC == 8)) ) {
				
			}
			break;
		break;
		
		case BMSHV_VCell:
			if( (RxHeader.DLC == 8)) ) {
				
			}
			break;
		break;
		
		case BMSHV_VBattery:
			if( (RxHeader.DLC == 8)) ) {
				
			}
			break;
		break;

}
