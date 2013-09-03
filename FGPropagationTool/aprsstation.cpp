#include "aprsstation.h"

AprsStation::AprsStation()
{
}

QString AprsStation::getImage()
{
    if(symbol == ">")
        return "14_1";
    if(symbol == "<")
        return "12_1";
    if(symbol == "#")
        return "3_0";
    if(symbol == "&")
        return "6_0";
    if(symbol == "W")
        return "7_3";
    if(symbol == "=")
        return "13_1";
    if(symbol == "r")
        return "2_5";
    if(symbol == "$")
        return "4_0";
    if(symbol == "I")
        return "9_2";
    if(symbol == "Y")
        return "9_3";
    if(symbol == "y")
        return "9_5";
    if(symbol == "b")
        return "2_4";
    if(symbol == "h")
        return "8_4";
    if(symbol == "d")
        return "4_4";
    if(symbol == "f")
        return "6_4";
    if(symbol == "a")
        return "1_4";
    if(symbol == "p")
        return "0_3";
    if(symbol == ";")
        return "11_1";
    if(symbol == "l")
        return "12_4";
    if(symbol == "Q")
        return "1_3";
    if(symbol == "O")
        return "15_2";
    if(symbol == "s")
        return "3_5";
    if(symbol == "u")
        return "5_5";
    if(symbol == "v")
        return "6_5";
    if(symbol == "-")
        return "13_0";
    if(symbol == "?")
        return "15_1";
    if(symbol == "_")
        return "15_3";
    if(symbol == "0")
        return "0_1";
    if(symbol == "!")
        return "1_0";
    if(symbol == "[")
        return "11_3";
    if(symbol == "(")
        return "8_0";
    if(symbol == "'" || symbol=="^")
        return "7_0";

    return "13_6";

}
