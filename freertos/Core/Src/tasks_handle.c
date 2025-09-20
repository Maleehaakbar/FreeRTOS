
#include "main.h"

const char* inv_option = "///Invalid option///\n";
void process_command (command_t* command);

void timer_start(int n);
void timer_stop();
void print_time_date();
uint8_t getnumber(uint8_t *p , int len);
void rtc_configure_time(RTC_TimeTypeDef *time);

void menu_task(void *parameters)
{
    (void) parameters; 
    uint32_t cmd_addr; 
    uint8_t option;
    command_t* cmd;
    const char* menu = "\n===================\n"
                       "|       Menu        |\n"
                       "\n===================\n"
                       "LED effect    ----> 0\n"
					   "Date and time ----> 1\n"
					   "Exit          ----> 2\n"
					   "Enter your choice here : ";

    while(1){
       xQueueSendToBack(xQueue2,(void*)&menu, portMAX_DELAY);  //pass the pointer(double pointer) as queue holds pointers to items
       xTaskNotifyWait( 0,0,&cmd_addr,portMAX_DELAY);
    cmd = (command_t*)cmd_addr;             //cmd_addr is raw address , typecst it back to struct
    if (cmd->len == 1)
    { 
        //option = cmd->payload[0] - 48;
        option = cmd->payload[0];
        switch(option)
        {
            case '0':
                curr_state = Led_state;
                xTaskNotify(handle_LED_task,0,eNoAction);
                break;
            case '1':
                curr_state = RtcMenu;
                xTaskNotify(handle_RTC_task,0,eNoAction);
                break;
            case '2':
                break;
            default:
                xQueueSendToBack(xQueue2,&inv_option, portMAX_DELAY); 
                continue; 
        }

    }

    else
    {
        xQueueSendToBack(xQueue2,&inv_option, portMAX_DELAY); 
        continue;
    }

    xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
    }
}

void cmd_task(void *parameters)
{
    (void) parameters; 
    command_t cmd;

    while(1){

        xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
        process_command (&cmd);

    }
}

void print_task(void *parameters)
{   
    (void) parameters; 
    char* rcv_buff;
    while(1){
    xQueueReceive(xQueue2,&rcv_buff,portMAX_DELAY );
    HAL_UART_Transmit(&huart2,(uint8_t*)rcv_buff,strlen(rcv_buff),HAL_MAX_DELAY);  //UART evaluate size of data, not pointer.
    }
}

void LED_task(void *parameters)
{ 
    (void) parameters; 
    command_t *cmd;
    uint32_t cmd_addr;
    uint8_t led_option;
     const char* led_menu = "\n===========\n"
                            "|  LED_Menu |\n"
                            "\n===========\n"
                            "c1   ---->  0\n"
					        "c2   ---->  1\n"
					        "none ---->  2\n"
					        "Enter your choice here : ";

    while(1){
        xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
        xQueueSendToBack(xQueue2,(void*)&led_menu, portMAX_DELAY); 
        xTaskNotifyWait( 0,0,&cmd_addr,portMAX_DELAY);
        cmd = (command_t*)cmd_addr; 
        if(cmd->len < 4)
        {    
            led_option = cmd->payload[0];
            switch(led_option){
                case '0':
                    timer_start(0);
                    break;
                case '1':
                    timer_start(1);
                    break;
                case '2':
                    timer_stop();
                    break;
                default:
                    break;

            }
        }
        else
        {
            xQueueSendToBack(xQueue2,&inv_option, portMAX_DELAY); 
            curr_state = MainMenu;
        }
        xTaskNotify(handle_menu_task,0,eNoAction);

    }
}

void RTC_task(void *parameters)
{
    (void) parameters; 
    command_t *cmd;
    uint32_t cmd_addr;
    uint8_t rtc_option;
    static int rtc_state = HOUR_CONFIG;
    static RTC_TimeTypeDef time;
    const char* msg_rtc = "========================\n"
							"|         RTC          |\n"
							"========================\n"
                            "Configure Time   ----> 0\n"
							"Enable reporting ----> 1\n"
                            "Exit             ----> 2\n"
							"Enter your choice here : ";

    const char *msg_rtc_hh = "Enter hour(1-12):";
	const char *msg_rtc_mm = "Enter minutes(0-59):";
	const char *msg_rtc_ss = "Enter seconds(0-59):";

    while(1){
        xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
        xQueueSendToBack(xQueue2,(void*)&msg_rtc,portMAX_DELAY); 
        print_time_date();
        while(curr_state != MainMenu)
        {
            xTaskNotifyWait( 0,0,&cmd_addr,portMAX_DELAY);
            cmd = (command_t*)cmd_addr; 

            switch (curr_state)
            {
                case RtcMenu:
                {
                    if (cmd->len==1)
                    {   
                        rtc_option = cmd->payload[0];
                        switch(rtc_option)
                        {
                            case '0':
                                xQueueSendToBack(xQueue2,(void*)&msg_rtc_hh,portMAX_DELAY); 
                                curr_state = RtcTimeConfig;
                                break;
                            case '1':
                                break;
                        }

                    }

                    else 
                    {
                        xQueueSendToBack(xQueue2,&inv_option, portMAX_DELAY); 
                        xTaskNotify(handle_menu_task,0,eNoAction);
                    }
                }
                break;
                case RtcTimeConfig:
                {
                    switch(rtc_state)
                    {
                        case HOUR_CONFIG:
                        {
                            uint8_t hour = getnumber(cmd->payload, cmd->len);
                            time.Hours = hour;
                            rtc_state = MINUTES_CONFIG;
                            xQueueSendToBack(xQueue2,(void*)&msg_rtc_mm,portMAX_DELAY); 
                            break;
                        }
                        case MINUTES_CONFIG:
                        {
                            uint8_t minutes =  getnumber(cmd->payload, cmd->len);
                            time.Minutes = minutes;
                            rtc_state = SECONDS_CONFIG;
                            xQueueSendToBack(xQueue2,(void*)&msg_rtc_ss,portMAX_DELAY); 
                            break;
                        }
                        case SECONDS_CONFIG:
                        {
                            uint8_t seconds =  getnumber(cmd->payload, cmd->len);
                            time.Seconds = seconds;
                            rtc_configure_time(&time);
                            curr_state = MainMenu;
                            rtc_state = HOUR_CONFIG;
                            print_time_date();
                            break;
                        }
                        default:

                            xQueueSendToBack(xQueue2,&inv_option, portMAX_DELAY); 
                            xTaskNotify(handle_menu_task,0,eNoAction);
                            curr_state = MainMenu;
                            break;
                    }
                    break;         
                }
                default:
                    break;
            }

        }

    }
}

void process_command (command_t* command)
{
    uint8_t temp;
    uint8_t i=0;
    UBaseType_t status;

    status = uxQueueMessagesWaiting(xQueue1);

    if (status !=0)
    {
        do{     
            xQueueReceive(xQueue1,&temp,0);
            command->payload[i++] =  temp;
        }while(temp!= '\n');

    command->payload[i-1] ='\0';
    command->len = i-1;
    
    }   

    else
    {
        return;
    }

    switch(curr_state)
    {
        case MainMenu:
            xTaskNotify(handle_menu_task,(uint32_t)command,eSetValueWithOverwrite ); 
            break;
        case Led_state:
            xTaskNotify(handle_LED_task,(uint32_t)command,eSetValueWithOverwrite );
            break;
        case RtcMenu:
        case RtcTimeConfig:
        case RtcReport:
            xTaskNotify(handle_RTC_task,(uint32_t)command,eSetValueWithOverwrite );
            break;
        default:
            break;

    }
      

}

void LED_control_1(){
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void LED_control_2(){
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void timer_start(int n){
    timer_stop();
    xTimerStart( xTimers[n], portMAX_DELAY );
}

void timer_stop(){
    for (int i=0; i<NUM_TIMERS; i++)
    {
        xTimerStop( xTimers[i], portMAX_DELAY );
    }
   
}

void print_time_date(){

    static char time_arr[40];
    static char date_arr[40];
    
    static char* time_ptr = time_arr;
    static char* date_ptr = date_arr;
    char* format;

    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    
    HAL_RTC_GetTime(&hrtc, &time,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date,RTC_FORMAT_BIN);

    format = (time.TimeFormat == RTC_HOURFORMAT12_AM) ? "AM" : "PM";
     
    sprintf(((char*)time_arr), "\ntime is  %02d:%02d:%02d [%s]", time.Hours, time.Minutes, time.Seconds, format);
    xQueueSendToBack(xQueue2,(void*)&time_ptr,portMAX_DELAY); 

    sprintf(((char*)date_arr), "\ndate is  %02d-%02d-%02d ", date.Date, date.Month, date.Year);
    xQueueSendToBack(xQueue2,(void*)&date_ptr,portMAX_DELAY); 

}

uint8_t getnumber(uint8_t *p , int len)
{

	int value ;

	if(len > 1)
	   value =  ( ((p[0]-48) * 10) + (p[1] - 48) );
	else
		value = p[0] - 48;

	return value;

}

void rtc_configure_time(RTC_TimeTypeDef *time)
{

	time->TimeFormat = RTC_HOURFORMAT12_AM;
	time->DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
	time->StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&hrtc,time,RTC_FORMAT_BIN);
}
