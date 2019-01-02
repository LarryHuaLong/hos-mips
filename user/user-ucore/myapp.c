#include <ulib.h>
#include <stdio.h>
#include <string.h>
#include <file.h>
#include <unistd.h>
//--------------------------------------------
// Marco definitions
//--------------------------------------------
// 00-10: 0.984808f;
// 10-35: 0.819852f;
// 35-55: 0.573576f;
// 55-70: 0.343420f;
// 70-85: 0.087156f;
// 85-90: 1.000000f;

// 0.000000f * 524288 (2^22)
#define PRESET_DIVIDER_0    0
// 0.087156f * 524288 (2^22)
#define PRESET_DIVIDER_1    45695
// 0.343420f * 524288 (2^22)
#define PRESET_DIVIDER_2    179317
// 0.573576f * 524288 (2^22)
#define PRESET_DIVIDER_3    300719
// 0.819852f * 524288 (2^22)
#define PRESET_DIVIDER_4    429472
// 0.984808f * 524288 (2^22)
#define PRESET_DIVIDER_5    516323
// 1.000000f * 524288 (2^22)
#define PRESET_DIVIDER_6    524288

// index (1, 2, 3, 4)
// forward (0, 1)
// speed (0, 1, 2, 3)
#define WHEEL_SIG(index, forward, speed) (((speed << 8) | (forward ? 0x02 : 0x01)) << ((index - 1) << 1))

//--------------------------------------------
// Function declarations
//--------------------------------------------
void Mode0(int btData);
void Mode1(int btData);

void Idle(void);
void Move(unsigned int isForward, unsigned int isRight, unsigned int presetIndex);
void Move1(unsigned int lightSensorData);

void Delay(void);

int atoi(char*);
int bluetooth_int_handler(uint32_t, int);
//--------------------------------------------
// Function definitions
//--------------------------------------------
void Mode0(int btData)
{
    int x = (int)(btData >> 16);
    int y = (int)((btData << 16) >> 16);
    if (btData != 0)
    fprintf(1, "[data] %d [x] %d [y] %d\n\r", btData, x, y);

    static unsigned int idleCounter = 0;

    // Do coordinates validation
    if ((-20 <= x && x <= 20) && (-20 <= y && y <= 20))
    {
        if (idleCounter == 10)
        {
            Idle();
        }
        else
        {
            ++idleCounter;
        }
        return ;
    }
    else
    {
        idleCounter = 0;
    }

    // Clamp x,y to [-80, 80]
    if (x <= -80)
    {
        x = -80;
    }
    else if (x >= 80)
    {
        x = 80;
    }

    if (y <= -80)
    {
        y = -80;
    }
    else if (x >= 80)
    {
        y = 80;
    }

    unsigned int isForward = 0;
    unsigned int isRight = 0;
    unsigned int presetIndex = 0;

    // Left or Right
    if (x > 0)
    {
        isRight = 0;
    }
    else // if (x <= 0)
    {
        x = -x;
        isRight = 1;
    }

    // Forward or Backward
    if (y > 0)
    {
        isForward = 1;
    }
    else // if (y <= 0)
    {
        isForward = 0;
    }

    // Direction
    unsigned int cosineSquared = ((x*x) << 19) / (x*x + y*y);

    if (PRESET_DIVIDER_0 <= cosineSquared && cosineSquared <= PRESET_DIVIDER_1)
    {
        presetIndex = 0;
    }
    else if (PRESET_DIVIDER_1 <= cosineSquared && cosineSquared <= PRESET_DIVIDER_2)
    {
        presetIndex = 1;
    }
    else if (PRESET_DIVIDER_2 <= cosineSquared && cosineSquared <= PRESET_DIVIDER_3)
    {
        presetIndex = 2;
    }
    else if (PRESET_DIVIDER_3 <= cosineSquared && cosineSquared <= PRESET_DIVIDER_4)
    {
        presetIndex = 3;
    }
    else if (PRESET_DIVIDER_4 <= cosineSquared && cosineSquared <= PRESET_DIVIDER_5)
    {
        presetIndex = 4;
    }
    else // if (PRESET_DIVIDER_5 <= cosineSquared && cosineSquared <= PRESET_DIVIDER_6)
    {
        presetIndex = 5;
    }

    Move(isForward, isRight, presetIndex);
}

void Mode1(int btData)
{
	unsigned int m = sys_gpio_rw(0,0,0);
	m=m & 0x0000001f;
    // fprintf(1,"m=%u\n\r",m);

	switch(m)
	{
        case 16://10000
            //Move(1, 0, 7);
            //break;
        case 24://11000
		//case 25://11001
        case 28://11100
		case 29://11101
        case 30://11110
			Move(1, 0, 7);
			break;
        case 1://00001
            //Move(1, 1, 7);
            //break;
        case 3://00011
		case 7://00111
		case 15://01111
		//case 19://10011
	    case 23://10111
            Move(1, 1, 7);
			break;
        // case 0://00000
        // case 27://11011
        // case 17://10001
		default:
            Move(1,1,6);

		/*case 1://00001
			Move(1,0,5);
			break;
		//case 2://00010
		//	Move(1,0,5);
		//	break;
		case 3://00011
			Move(1,0,5);
			break;
		case 4://00100
			Move(1,1,6);
			break;
		case 6://00110
			// Move(1,0,5);
            Move(1,1,6);
			break;
        case 7://00111
			Move(1,0,5);
			break;
		//case 8://01000
		//	Move(1,1,5);
		//	break;
		case 12://01100
			//Move(1,1,5);
            Move(1,1,6);
			break;
        case 14://01110
			//Move(1,1,6);
            Move(1,1,6);
			break;
        case 15://01111
			Move(1,0,5);
			break;
		 case 16://10000
			Move(1,1,5);
			break;
		case 24://11000
			Move(1,1,5);
			break;
        case 28://11100
			Move(1,1,5);
			break;
        case 30://11110
			Move(1,1,5);
			break;
		default:
            fprintf(1,"1\r\n");
            Move(1,1,6);*/
	}
}

void Idle(void)
{
    sys_gpio_rw(1, 1,
        WHEEL_SIG(1, 1, 0)
        | WHEEL_SIG(2, 1, 0)
        | WHEEL_SIG(3, 1, 0)
        | WHEEL_SIG(4, 1, 0));
}

void Move(unsigned int isForward, unsigned int isRight, unsigned int presetIndex)
{
    unsigned int wheelSignal;

    switch (presetIndex)
    {
    case 0:
        wheelSignal = WHEEL_SIG(1, isForward, 3)
            | WHEEL_SIG(2, isForward, 3)
            | WHEEL_SIG(3, isForward, 3)
            | WHEEL_SIG(4, isForward, 3);
        break;
    case 1: // spin (25)
        if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 1, 0)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 1, 0);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 0)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 0, 0)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 3);
        }
        /*if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 0)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 0)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 0)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 0);
        }*/
        break;
    case 2: // spin (10)
        if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 1)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 1, 0)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 1)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 1, 0);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 0)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 1)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 0, 0)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 1);
        }
        /*if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 0)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 1, 1)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 0)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 1, 1);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 1)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 0)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 0, 1)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 0);
        }*/
        break;
    case 3: // spin (5)
        if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 1)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 1, 0)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 0, 1)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 1, 1);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 0)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 1, 1)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 0, 0)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 1, 1);
        }
        /*if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 0)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 0, 1)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 0)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 0, 1);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 1)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 0)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 1, 1)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 0);
        }*/
        break;
    case 4: // spin (0) back
        if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 0)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 0)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 0, 3);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 0)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 0);
        }
        /*if (isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 0)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 1, 3);
        }
        else if (isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 1, 0)
                | WHEEL_SIG(3, 1, 3)
                | WHEEL_SIG(4, 0, 3);
        }
        else if (!isForward && isRight)
        {
            wheelSignal = WHEEL_SIG(1, 1, 3)
                | WHEEL_SIG(2, 0, 3)
                | WHEEL_SIG(3, 0, 0)
                | WHEEL_SIG(4, 0, 3);
        }
        else // if (!isForward && !isRight)
        {
            wheelSignal = WHEEL_SIG(1, 0, 3)
                | WHEEL_SIG(2, 1, 3)
                | WHEEL_SIG(3, 0, 3)
                | WHEEL_SIG(4, 0, 0);
        }*/
        break;      
    case 5: // turn around
        wheelSignal = WHEEL_SIG(1, isRight ^ 1, 2)
            | WHEEL_SIG(2, isRight, 2)
            | WHEEL_SIG(3, isRight ^ 1, 2)
            | WHEEL_SIG(4, isRight, 2);
        break;
    case 6:
        wheelSignal = WHEEL_SIG(1, isForward, 1)
            | WHEEL_SIG(2, isForward, 1)
            | WHEEL_SIG(3, isForward, 1)
            | WHEEL_SIG(4, isForward, 1);
        break;
    case 7: // turn around
        wheelSignal = WHEEL_SIG(1, isRight ^ 1, 1)
            | WHEEL_SIG(2, isRight, 1)
            | WHEEL_SIG(3, isRight ^ 1, 1)
            | WHEEL_SIG(4, isRight, 1);
        break;
    }
    
    sys_gpio_rw(1, 1, wheelSignal);
    fprintf(1, "[wheelSignal] %x\n\r\n\r", wheelSignal);
}

void Delay(void)
{
    volatile unsigned int j;
    for (j = 0; j < (1000); j++) ; // delay
}

int atoi(char *str)
{
        if(!str)
                return -1;
        bool bMinus=0;
        int result=0;
 
        if(('0'>*str || *str>'9')&&(*str=='+'||*str=='-'))
        {
               if(*str=='-')
                bMinus=1;
               *str++;
        }
        while( *str != '\0')
        {
                if('0'> *str || *str>'9')
                        break;
                else
                        result = result*10+(*str++ - '0');
        }
 
        if (*str != '\0')//no-normal end
                return -2;
 
        return bMinus?-result:result;
}

int bluetooth_int_handler(uint32_t mode, int fd)
{
    // fprintf(1, "mode 1\n\r");
    if(mode == 1){   // redlight
        Mode1(1);
        return 1;
    }
    else if(mode == 2){  // ultrasonic
        // fprintf(1, "mode 2\n\r");  
        Mode0(sys_read_bt());
        return 2;
    }
    char base[1025]="";
    int ret=0;
    // fprintf(1,"fd = %d/n/r",fd);
    ret = read(fd, base,1024);
    if (!ret)
    {
        //fprintf(1,"no data to read\r\n");
        return -1;
    }
    fprintf(1, "%s\n\r", base);
    int pos = -1;
    int i;
    for(i = 1023; i >= 0; i--){
        if(base[i] == ';' || i == 0){
            if(pos != -1 || i == 0){
                pos = i;
                break;
            }
            else pos = 0;
        }
    }
    if(pos == -1)return -1;
    // fprintf(1, "pos: %d\n\r", pos);
    pos++;
    char bt_command[30];
    for(i = 0; i < 30; i++){
        if(base[pos] == ';'){
            bt_command[i] = '\0';
            break;
        }
        bt_command[i] = base[pos++];
    }
    // fprintf(1, "command: %s\n\r", bt_command);
    // new
    if(bt_command[1] == 'M')  // gravity
    {
        int tpos = 0, yn = 0, x = 0, y = 0;
        int i = 0;
        while(bt_command[i] != '\0'){
            if(bt_command[i] == ','){
                char temp[5];
                i++;
                while(i < pos && bt_command[i] != '.'){
                    temp[tpos++] = bt_command[i++];
                };
                temp[tpos] = '\0';
                if(yn == 0) {
                    yn = 1;
                    x = atoi(temp);
                }
                else {
                    yn = 0;
                    y = atoi(temp);
                }
                tpos = 0;
            }
            else i++;
        }
        uint32_t res = 65535;
        res = res & y;
        x = x << 16;
        res = res | x;

        Mode0(res);
        return 0;
    }
    /* else if(bt_command[1] == 'J') // rocker
    {
        bt_command[pos]='\0';
        kprintf("bt_command: %s\n\r", bt_command);
        // kprintf("rocker\n\r");
        // store the data
        bt_data[1] = my_rocker();
        bt_data[0] = 1;
    } 
    */
    else if(bt_command[1] == 'W' && bt_command[0] == 'H') // automatic tracking
    {
        uint32_t res = 255;
        res = (res << 8) | (res << 24);
        Mode0(res);
        return 1;
    }
    else if(bt_command[2] == 'M' && bt_command[1] == 'R' && bt_command[0] == 'U'){
        return 2;
    }
    // new
    //fprintf(1, "Idle\n\r");
    Idle();

    return 0;
}

//--------------------------------------------
// Main entry
//--------------------------------------------
int main(int argc, char **argv)
{
    uint32_t bt_mode = 0;  // 0:bluetooth, 1:redlight, 2:ultrasonic
    int fd = open("bluetooth:",O_RDONLY);
    while(1){

        int x = bluetooth_int_handler(0,fd);
        if(x == -1)bluetooth_int_handler(bt_mode,fd); 
        else {
            bt_mode = x;
        }
        sleep(8);
    }
    return 0;
/* 
    unsigned int lightSensorData = 0x00;
    unsigned int mode = 0x00;
    int btData;

    while (1)
    {
        mode = 1;
        btData = sys_read_bt();

        if (mode == 0)
        {
            Mode0(btData);
        }
        else if (mode == 1)
        {
            Mode1(btData);
        }

        Delay();
    }

	return 0; */
}
