#ifndef CHRONODATE_H
#define CHRONODATE_H

#include <chrono>
#include <compare>

class ChronoDate
{
public:
public:
    ChronoDate(int year, unsigned month, unsigned day);
    ChronoDate(std::chrono::year_month_day ymd);
    ChronoDate() = default;

    int year() const;
    unsigned month() const;
    unsigned day() const;
    int serial_date() const;
    std::chrono::year_month_day ymd() const;

    ChronoDate& add_years(int rhs_years);
    ChronoDate& add_months(int rhs_months);
    ChronoDate& add_days(int rhs_days);
    ChronoDate& weekend_roll();

    int operator - (const ChronoDate& rhs) const;
    bool operator == (const ChronoDate& rhs) const;
    std::strong_ordering operator <=> (const ChronoDate& rhs) const;

    bool is_end_of_month() const;
    unsigned days_in_month() const;
    bool is_leap_year() const;

private:
    std::chrono::year_month_day date_
        {std::chrono::year{1970}, std::chrono::month{1}, std::chrono::day{1}};

    void validate_() const;
};

#endif // CHRONODATE_H
