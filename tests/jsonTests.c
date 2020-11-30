#include <stdio.h>
#include <string.h>

#include "json.h"


static const char *validJSON[] = {
    "{\"key\" : \"value\"}",
    "{\"key\" : \"ThisIsAveryLongStringThatINeedToMakeVeryLong\"}",
    "{\"key\" : 1234 }",
    "{\"key\" : [ 1, 2, 3] }",
    "{}",
    "{[]}",
    "{\"key\" : true}",
    "{\"key\" : false}",
    "{\"key\" : null}",
    "{\"key\" : { }}",
    "{\"key\" : {[]}}",
    "{\"key\" : { [ ] } }",
    "{\"key\" : {\"childKey\" : 123}}"
    "{\"key\" : {\"childKey\" : \"childStringValue\"}}",
    "{\"key\" : {\"childKey\" : [ \"array\", \"of\", \"strings\", \"as\", \"child\", \"key\", \"values\"]}}",
    "{\"key\" : \"key\"}",
    "{\"key\" : [ true, true, false, false]}",
    "{\"key\" : \"true\"}",
    "{\"key\" : \"false\"}",
    "{\"key\" : \"null\"}",
    "{\"true\" : \"true\"}",
    "{\"false\" : \"false\"}",
    "{\"null\" : \"null\"}",
    "{\"hexValueKey\" : \\uffff}",
    "{\"hexValueKey\" : \\uFFFF}",
    "{\"hexValueKey\" : \\u0000}",
    "{\"hexValueKey\" : \\uabcd}",
    "{\"hexValueKey\" : \\uABCD}",
    "{\"hexValueStringifiedKey\" : \"\\uffff\"}",
    "{\"hexValueStringifiedKey\" : \"\\u0000\"}",
    "{\"hexValueStringifiedKey\" : \"\\uFFFF\"}",
    "{\"hexValueStringifiedKey\" : \"\\uABCD\"}",
    "{\"decimalValueKey\" : 123.456}",
    "{\"decimalValueString\" : \"123.456\"}",
    "{\"exponentValue\": 1.001e-9}",
    "{\"exponentValue\": 1.001e9}",
    "{\"exponentValue\": 1.001e+9}",
    "{\"exponentValue\": 1.001E-9}",
    "{\"exponentValue\": 1.001E9}",
    "{\"exponentValue\": 1.001E+9}",
    "{\"exponentValue\": 1e-9}",
    "{\"exponentValue\": 1e9}",
    "{\"exponentValue\": 1e+9}",
    "{\"exponentValue\": 1E-9}",
    "{\"exponentValue\": 1E9}",
    "{\"exponentValue\": 1E+9}",
    "{\"differentTypesOfWhiteSpace\"\t:\t\"valueString\"}",
    "{\"recursion\" : {\"into\" : {\"child\" : {\"objects\" : {\"key\" : [ \"this\", \"is\", \"an\", \"array\", \"that\", \"is\", \"deeply\", \"nested\"]}}}}}",
    "{\"key\" : {[{\"arrKey\" : \"arrValue\"}]}}",
    "{\"key\" : [1]}",
    "{\"key\" : [\"1\"]}",
    "{\"key1\" : [{\"arrJsonKey1\" : \"arrJsonVal1\"}, {\"arrJsonKey2\" : \"arrJsonVal2\"}]}",
};


static jsontok_t tokens[200];

int main(void){
    printf("\nTesting json parser against valid jsons\n");
    for(unsigned int i = 0; i < sizeof(validJSON)/sizeof(*validJSON); i++)
    {   
        json_parser p = json_init();
        printf("%s ... ", validJSON[i]);
        jsonerr_t status = json_parse(&p, validJSON[i], strlen(validJSON[i]), tokens, sizeof(tokens)/sizeof(*tokens));
        if(status < 0)
        {   
            printf("failed");
            return 1;
        }
        else
        {
            printf("passed");
        }
        printf("\n");
    }
    return 0;
}
