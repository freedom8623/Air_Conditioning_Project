#include "KEY\key.h"


unsigned char Key_Read(void)
{
    unsigned char key_value = 0;

    if(P33 == 1)   //S5
        key_value = 1;
		
    if(P35 == 1)   //S7
        key_value = 2;

    if(P31 == 1)   //S3
        key_value = 3;

    if(P30 == 1)   //S2 OFF\ON
        key_value = 5;
		
    if(P32 == 1)   //S4 R
        key_value = 6;
		

		
    return key_value;
}

