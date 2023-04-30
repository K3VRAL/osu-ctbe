#include "main.hpp"

// https://www.youtube.com/watch?v=L271J_lKyRA
void press_key(char ch)
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    input.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(ch)), 0);
    SendInput(1, &input, sizeof(input));
    Sleep(10);
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
}
