#include <iostream>
#include "blackscholes.h"
#include "payoff.h"
#include "mypair.h"

#include <chrono>

using namespace std;
using namespace std::chrono;


int main()
{
    year_month_day ymd_03{year{2022}, month{10}, day{7}};
    auto add_days = sys_days(ymd_03) + days(3);

    ymd_03 = add_days;

    cout << ymd_03 << endl;

    return 0;
}
