#include <stdio.h>
#include <string.h>

#include "jtok.h"

static const char *validJSON[] = {
    "{\"key1\" : [{\"arrJsonKey1\" : \"arrJsonVal1\"}, {\"arrJsonKey2\" : "
    "\"arrJsonVal2\"}]}",

    /* primitive */
    "{\"key\" : true}",
    "{\"key\" : false}",
    "{\"key\" : null}",

    /* Strings */
    "{\"key\" : \"value\"}",
    "{\"key\" : \"ThisIsAveryLongStringThatINeedToMakeVeryLong\"}",
    "{\"key\" : \"true\"}",
    "{\"key\" : \"false\"}",
    "{\"key\" : \"null\"}",
    "{\"key\" : \"key\"}", /* duplicate string as value for key */
    "{\"false\" : \"false\"}",
    "{\"null\" : \"null\"}",
    "{\"true\" : \"true\"}",

    /* numbers */
    "{\"decimalValueKey\" : 123.456}",
    "{\"key\" : 1234 }",
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
    "{\"decimalValueString\" : \"-123.456\"}",
    "{\"exponentValue\": -1.001e-9}",
    "{\"exponentValue\": -1.001e9}",
    "{\"exponentValue\": -1.001e+9}",
    "{\"exponentValue\": -1.001E-9}",
    "{\"exponentValue\": -1.001E9}",
    "{\"exponentValue\": -1.001E+9}",
    "{\"exponentValue\": -1e-9}",
    "{\"exponentValue\": -1e9}",
    "{\"exponentValue\": -1e+9}",
    "{\"exponentValue\": -1E-9}",
    "{\"exponentValue\": -1E9}",
    "{\"exponentValue\": -1E+9}",

    /* escaped hex values */
    "{\"hexValueStringifiedKey\" : \"\\uffff\"}",
    "{\"hexValueStringifiedKey\" : \"\\u0000\"}",
    "{\"hexValueStringifiedKey\" : \"\\uFFFF\"}",
    "{\"hexValueStringifiedKey\" : \"\\uABCD\"}",

    /* Child objects */
    "{}",
    "{\"key\" : {}}", /* empty object */
    "{\"key\" : {\"childKey\" : 123}}",
    "{\"key\" : {\"childKey\" : \"childStringValue\"}}",
    "{\"key\" : [ true, true, false, false]}",
    "{\"key\" : { }}",

    /* Arrays */
    "{\"key\" : [ 1, 2, 3] }",
    "{\"key1\" : [{\"arrJsonKey1\" : \"arrJsonVal1\"}, {\"arrJsonKey2\" : "
    "\"arrJsonVal2\"}]}",
    "{\"key\" : [\"1\"]}",
    "{\"key\" : [1]}",
    "{\"key\" : []}", /* empty array */

    /* Mixing object types */
    "{\"recursion\" : {\"into\" : {\"child\" : {\"objects\" : {\"key\" : [ "
    "\"array\", \"that\", \"is\", \"deeply\", \"nested\"]}}}}}",
    "{\"key\" : {\"key\" : [{\"arrKey\" : \"arrValue\"}]}}",
    "{\"key\" : {\"childKey\" : [ \"array\", \"of\", \"strings\", \"as\", "
    "\"child\", \"key\", \"values\"]}}",

    /* Misc */
    "{\"differentTypesOfWhiteSpace\"\t:\t\"valueString\"}",
    "{\n\"differentTypesOfWhiteSpace\"\n:\n\"valueString\"\n}",

};

static jtoktok_t tokens[200];
int              main(void)
{
    printf("\nTesting jtok parser against valid jsons\n");
    for (unsigned int i = 0; i < sizeof(validJSON) / sizeof(*validJSON); i++)
    {
        jtok_parser_t p = jtok_new_parser(validJSON[i]);
        printf("\n%s ... ", validJSON[i]);
        JTOK_PARSE_STATUS_t status =
            jtok_parse(&p, tokens, sizeof(tokens) / sizeof(*tokens));
        if (status < 0)
        {
            printf("test failed. (status == %d)", status);
            return 1;
        }
        else
        {
            printf("test passed. ");
        }
        printf("\n");
    }
    return 0;
}