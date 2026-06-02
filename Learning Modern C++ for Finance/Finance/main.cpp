#include <iostream>
#include "blackscholes.h"
#include "payoff.h"
#include "mypair.h"

#include<vector>
#include<deque>
#include<ranges>
#include<algorithm>
#include<iterator>

using namespace std;

class Quadratic{
public:
    Quadratic(double a, double b, double c): a_{a}, b_{b}, c_{c} {}

    double operator()(double x) const{
        return (a_ * x + b_) * x + c_;
    }

    double value(double x) const{
        return (a_ * x + b_) * x + c_;
    }

private:
    double a_, b_, c_;
};


int main(){
    Quadratic q{2.0, 4.0, 2.0};
    std::vector<double> v{-1.4, -1.3, -1.2, -1.1, 0.0, 1.1, 1.2, 1.3, 1.4};
    std::deque<double> y;
    auto quad = [&q](double x){
        return q.value(x);
    };

    std::ranges::transform(v, std::back_inserter(y), quad);


    return 0;
}
