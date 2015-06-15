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

#pragma once

// ___________________________________________________________________________//

#include <boost/python.hpp>

// ___________________________________________________________________________//

#include "integration.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

struct Path: lagrangian::Path, bp::wrapper<lagrangian::Path>
{
    Path(lagrangian::Path const & arg);

    Path(lagrangian::JulianDay const & start_time,
            lagrangian::JulianDay const & end_time,
            boost::posix_time::time_duration const & delta_t,
            lagrangian::Field* field);

    bp::tuple WrapperCompute(const lagrangian::Iterator& it,
            const double x0,
            const double y0) const;
};

struct FiniteLyapunovExponents: lagrangian::FiniteLyapunovExponents, bp::wrapper<
        lagrangian::FiniteLyapunovExponents>
{
    FiniteLyapunovExponents(lagrangian::FiniteLyapunovExponents const & arg);

    FiniteLyapunovExponents(lagrangian::JulianDay const & start_time,
            lagrangian::JulianDay const & end_time,
            boost::posix_time::time_duration const & delta_t,
            lagrangian::FiniteLyapunovExponents::Mode mode,
            double const min_separation,
            double const delta,
            lagrangian::Field* field);

    bp::tuple WrapperCompute(const lagrangian::Iterator& it,
            lagrangian::Position& p) const;
};

// ___________________________________________________________________________//

void IntegrationPythonModule();

}