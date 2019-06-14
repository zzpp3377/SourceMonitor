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
