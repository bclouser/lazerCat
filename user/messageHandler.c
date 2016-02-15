
#include "messageHandler.h"
#include "json/jsonparse.h"
#include "json/jsontree.h"
#include "osapi.h"

#include "servo.h"

/*This function needs to be cleaned up. I don't like that we are 
 instructing the servos from within this function. Maybe do it in interface up one level?
 This will change a lot in the future based on how our messages look
 so i am going to leave it alone for now */
bool handleMessage(char* messageBuf)
{
	struct jsonparse_state js;
	jsonparse_setup(&js, messageBuf, os_strlen(messageBuf));
	//int duty = jsonparse_get_value_as_int(js);
	char buf[32] = {0};
	int type = 0;
	while( (type = jsonparse_next(&js)) != JSON_TYPE_ERROR){
		switch(type){
			case JSON_TYPE_ARRAY:
				os_printf("type = JSON_TYPE_ARRAY\n");
				break;
			case JSON_TYPE_OBJECT:
				os_printf("type = JSON_TYPE_OBJECT\n");
				break;
			case JSON_TYPE_PAIR:
				os_printf("type = JSON_TYPE_PAIR\n");
				break;
			case JSON_TYPE_PAIR_NAME:
				os_printf("type = JSON_TYPE_PAIR_NAME\n");
				break;
			case JSON_TYPE_STRING:
				os_printf("type = JSON_TYPE_STRING\n");
				break;
			case JSON_TYPE_INT:
				os_printf("type = JSON_TYPE_INT\n");
				break;
			case JSON_TYPE_NUMBER:
				os_printf("type = JSON_TYPE_NUMBER\n");
				break;
			case JSON_TYPE_ERROR:
				os_printf("type = JSON_TYPE_ERROR\n");
				break;
			case JSON_TYPE_NULL:
				os_printf("type = JSON_TYPE_NULL\n");
				break;
			case JSON_TYPE_TRUE:
				os_printf("type = JSON_TYPE_TRUE\n");
				break;
			case JSON_TYPE_FALSE:
				os_printf("type = JSON_TYPE_FALSE\n");
				break;
			case JSON_TYPE_CALLBACK:
				os_printf("type = JSON_TYPE_CALLBACK\n");
				break;
			default:
			 	os_printf("unknown type\n");
		}

		// Found a key-value pair!
		if( type == JSON_TYPE_PAIR_NAME ){
			if(jsonparse_strcmp_value(&js, "servo1") == 0)
			{
				type = jsonparse_next(&js);
				// ok, so now look at the value
				type = jsonparse_next(&js);
				if( type == JSON_TYPE_STRING){
					jsonparse_copy_value(&js, buf, 32);
					os_printf("Here is the value: %s\n", buf);
				}
				else if(type == JSON_TYPE_INT || JSON_TYPE_NUMBER){
					jsonparse_copy_value(&js, buf, 32);
					os_printf("Here is the value: %s\n", buf);
					uint8 dutyPercent = jsonparse_get_value_as_int(&js);
					os_printf("Setting dutyPercent to be %d\n", dutyPercent);

					setServo(e_ServoNum1, dutyPercent);

				}
				else{
					os_printf("BAD PARSE! Couldn't find value associated with key!\n");
				}
			}
			else
			{
				os_printf("Hmmm, key value was not a string\n");
				return false;
			}
		}
	}
	return true;
}
