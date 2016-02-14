
#include "messageHandler.h"
#include "json/jsonparse.h"
#include "json/jsontree.h"
#include "osapi.h"

#include "servo.h"




bool handleMessage(char* messageBuf)
{
	struct jsonparse_state js;
	jsonparse_setup(&js, messageBuf, os_strlen(messageBuf));
	//int duty = jsonparse_get_value_as_int(js);
	char buf[32] = {0};
	int type = 0;
	while( (type = jsonparse_next(&js)) != JSON_TYPE_ERROR){
		/*
		#define JSON_TYPE_ARRAY '['
		#define JSON_TYPE_OBJECT '{'
		#define JSON_TYPE_PAIR ':'
		#define JSON_TYPE_PAIR_NAME 'N'
		#define JSON_TYPE_STRING '"'
		#define JSON_TYPE_INT 'I'
		#define JSON_TYPE_NUMBER '0'
		#define JSON_TYPE_ERROR 0
		*/
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
