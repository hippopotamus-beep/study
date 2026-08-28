#ifndef MCOPTIONVALUATION_H
#define MCOPTIONVALUATION_H

#include "optioninfo.h"

enum class BarrierType
{
    none,
    up_and_out,
    down_and_out
};

class MCOptionValuation
{
public:
    MCOptionValuation(OptionInfo&& opt, int time_steps, double vol, double int_rate,
        double div_rate = 0.0, BarrierType barrier_type = BarrierType::none,
        double barrier_value = 0.0);

    double calc_price(double spot, int num_scenarios, unsigned unif_start_seed);

private:
    OptionInfo opt_;
    int time_steps_;
    double vol_, int_rate_, div_rate_;
    BarrierType barrier_type_;
    double barrier_value_;
};

#endif // MCOPTIONVALUATION_H
