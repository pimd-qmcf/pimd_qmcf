/*****************************************************************************
<GPL_HEADER>

    PQ
    Copyright (C) 2023-now  Jakob Gamper

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

<GPL_HEADER>
******************************************************************************/

#include "internalQMRunner.hpp"

#include <thread>

using QM::InternalQMRunner;

void InternalQMRunner::run(pq::SimBox &simBox, pq::PhysicalData &physicalData)
{
    std::jthread timeoutThread{[this](const std::stop_token stopToken)
                               { throwAfterTimeout(stopToken); }};

    execute(simBox);
    collectData(simBox, physicalData);

    timeoutThread.request_stop();
}