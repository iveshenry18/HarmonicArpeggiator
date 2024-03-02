//
// Created by Henry Ives on 3/1/24.
//

#ifndef APICPPPROJECT_FRACTION_H
#define APICPPPROJECT_FRACTION_H

class Fraction
{
public:
    Fraction (int num, int denom) : numerator (num), denominator (denom)
    {
    }

    float operator*(float const& rhs) const {
        return (rhs * static_cast<float> (numerator)) / static_cast<float> (denominator);
    }

private:
    int numerator;
    int denominator;
};

#endif //APICPPPROJECT_FRACTION_H
