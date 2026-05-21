#include <iostream>
#include <array>
#include <cmath>

// using namespace std;

enum class PayoffType{
    Call = 1,
    Put = -1
};

class BlackScholes{
public:
    BlackScholes(double strike, double spot, double time_to_exp, PayoffType payoff_type,
                 double rate, double div = 0.0);
    double operator()(double vol);


private:
    std::array<double, 2> compute_norm_args(double vol);

    double strike_, spot_, time_to_exp_;
    PayoffType payoff_type_;
    double rate_, div_;
};


BlackScholes::BlackScholes(double strike, double spot, double time_to_exp,
                           PayoffType payoff_type, double rate, double div) :
    strike_{strike}, spot_{spot}, time_to_exp_{time_to_exp}, payoff_type_{payoff_type},
    rate_{rate}, div_{div}{}


double BlackScholes::operator ()(double vol){
    const int phi = static_cast<int>(payoff_type_);

    if(time_to_exp_ > 0.0){
        auto norm_args = compute_norm_args(vol);
        double d1 = norm_args[0];
        double d2 = norm_args[1];

        auto norm_cdf = [](double x){
            return (1.0 + std::erf(x/std::sqrt(2)))/2.0;
        };

        double nd_1 = norm_cdf(phi * d1);
        double nd_2 = norm_cdf(phi * d2);
        double disc_fctr = exp(-rate_ * time_to_exp_);

        return phi * (spot_ * exp(-div_ * time_to_exp_)* nd_1 -
               disc_fctr * strike_ * nd_2);
    }else{
        return std::max(phi * (spot_ - strike_), 0.0);
    }
}

std::array<double, 2> BlackScholes::compute_norm_args(double vol){
    double numer = log(spot_ / strike_)
                   + (rate_ - div_ + 0.5 * vol * vol) * time_to_exp_;
    double d1 = numer / (vol * sqrt(time_to_exp_));
    double d2 = d1 - vol * sqrt(time_to_exp_);
    return std::array<double, 2>{d1, d2};
}

int main()
{
    double strike = 75.0;
    auto payoff_type = PayoffType::Call;
    double spot = 100.0;
    double rate = 0.05;
    double vol = 0.25;
    double time_to_exp = 0.0;

    BlackScholes bsc_itm_exp{strike, spot, time_to_exp, payoff_type, rate};

    double value = bsc_itm_exp(vol);

    std::cout << value << std::endl;

    time_to_exp = 0.25;
    double dividend = 0.075;
    payoff_type = PayoffType::Call;
    BlackScholes bsp_otm_tv{strike, spot, time_to_exp, payoff_type, rate, dividend};
    value = bsp_otm_tv(vol);

    std::cout << value << std::endl;

    return 0;
}
