/*
    This file is part of lagrangian library.

    lagrangian is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lagrangian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VONKARMAN_FIELD_HPP_
#define VONKARMAN_FIELD_HPP_

#include "field.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace field
{

class Vonkarman: public Field
{
private:
    double a_;
    double w_;
    double r0_;
    double tc_;
    double alpha2_;
    double y0_;
    double l_;
    double u0_;
public:
    Vonkarman(const double a = 1,
            const double w = 35.06,
            const double r0 = 0.35,
            const double tc = 1,
            const double alpha = 2,
            const double y0 = 0.3,
            const double l = 2,
            const double u0 = 14) :
        Field(kMetric), a_(a), w_(w), r0_(r0), tc_(tc), alpha2_(alpha * alpha),
                y0_(y0), l_(l), u0_(u0)
    {
    }
    bool Compute(const double t,
            const double x,
            const double y,
            double& u,
            double& v) const;
};

} // namespace field
} // namespace lagrangian

#endif /* VONKARMAN_FIELDS_HPP_ */
