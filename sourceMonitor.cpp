/*
 * sourceMonitor.cpp
 *
 * Copyright (c) 2019 Zhang Peng
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *USA.
 *
 */


#include "sourceMonitor.hpp"
#include "monitorImpl.hpp"

SourceMonitor::SourceMonitor(){
    monitorImpl=new MonitorImpl();
}

SourceMonitor::~SourceMonitor(){
    delete monitorImpl;
}

bool SourceMonitor::registerStatistic(StatisticT statistic){
    monitorImpl->registerStatistic(statistic);
}
bool SourceMonitor::registerPid(unsigned int pid){
    monitorImpl->registerPid(pid);
}
bool SourceMonitor::init(){
    return monitorImpl->init();
}
void SourceMonitor::update(){
    monitorImpl->update();
}
double SourceMonitor::getStatistic(unsigned int pid,StatisticT statistic){
    monitorImpl->getStatistic(pid,statistic);
}
