#include <stdio.h>
#include <string.h>

#include "jtok.h"

static const char *invalidJSON[] = {
    "{key : 123 }", /* unquoted key */
    "key : 123", /* no braces */
    "{\"key\"}", /* no value */
    "[ ]", /* no json */
    "\"key\"", /* quoted string without braces */
    "123",
    "{[\"123\", \"456\", \"abc\"]}",
    "{\"key\" : [\"123\" \"456\" \"abc\"]}", /* commas are missing from array */
    "{\"keyMissingSecondQuote : \"stringValue\"}",
    "{keyMissingFirstQuote\" : \"stringValue\"}",
    "{\"key\" : valueStringMissingFirstQuote\"}",
    "{\"key\" : \"valueStringMissingSecondQuote}",
    "{\"key\" : \\uxxxx}", /* invalid hex string */
    "{\"key\" : \"\\uxxxx\"}" /* invalid quoted hex string */
    "{\"key\" : {}", /* mismatched braces */
    "{\"key\" : }}", /* mismatched braces */
    "{ \"key\": [}", /* mismatched square brackets */
    "{\"key\":]}",   /* mismatched square brackets */
    "{123}", /* value without key */
    "{{\"childKey\" : \"childValue\"}}", /* child json with no key */
    "{[]}", /* no key for empty array */
    "{\"key\" : {[]}}", /* array with no key inside child object */

    /* ESCAPED HEX VALUES MUST BE INSIDE STRINGS */
    "{\"hexValueKey\" : \\uFFFF}",
    "{\"hexValueKey\" : \\u0000}",
    "{\"hexValueKey\" : \\uabcd}",
    "{\"hexValueKey\" : \\uABCD}",
    "{\"key\" : 12true}", /* valid primitive embedded in the invalid primitve */

    /* invalid exponent / primitive / decimal formats */
    "{\"key\" : .123}",
    "{\"key\" : e9}",
    "{\"key\" : e-9}",
    "{\"key\" : e+9}",
    "{\"key\" : e.9}",
    "{\"key\" : .123}",
    "{\"key\" : E9}",
    "{\"key\" : E-9}",
    "{\"key\" : E+9}",
    "{\"key\" : E.9}",
    "{\"key\" : 123.}",
    "{\"key\" : 123.e9}",
    "{\"key\" : 123.e+9}",
    "{\"key\" : 123.e-9}",
    "{\"key\" : 123.e9.}",
    "{\"key\" : 123.e-9.}",
    "{\"key\" : 123.e+9.}",
    "{\"key\" : 123.e+9.0}",
    "{\"key\" : 123.e-9.0}",
    "{\"key\" : 123.e9.0}",
    "{\"key\" : 123true}",
    "{\"key\" : 123false}",
    "{\"key\" : 123null}",
    "{\"key\" : true123}",
    "{\"key\" : false123}",
    "{\"key\" : null123}",


    /* Arrays */
    "{\"key\" : [,]}",
    "{\"key\" : [123 456 789]}",
    "{\"key\" : [123, ]}",
    "{\"key\" : [\"abc\", 123, {\"childkey\" : \"childval\"}]}", /* mixed types */

    /* Misc */
    "{\"key\" : }",
    "{: \"key\" }",
    "{\"\" : \"value\"}",
};

static jtoktok_t tokens[200];
int main(void){
    printf("\nTesting jtok parser against invalid jsons\n");
    for(unsigned int i = 0; i < sizeof(invalidJSON)/sizeof(*invalidJSON); i++)
    {   
        jtok_parser_t p = jtok_new_parser(invalidJSON[i]);
        printf("\n%s ... ", invalidJSON[i]);
        JTOK_PARSE_STATUS_t status = jtok_parse(&p, tokens, sizeof(tokens)/sizeof(*tokens));
        if(status >= 0)
        {   
            printf("test failed. Expected status < 0");
            return 1;
        }
        else
        {
            printf("test passed. status == %d", status);
        }
        printf("\n");
    }
    return 0;
}