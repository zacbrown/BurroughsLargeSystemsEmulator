#include "simplefs.h"
#include <iostream>

using namespace std;

int main(void)
{
    Block block;
    putInt(&block, 4, 55555555);
    cout << "Got " << extractInt(block, 4) << endl;
}
