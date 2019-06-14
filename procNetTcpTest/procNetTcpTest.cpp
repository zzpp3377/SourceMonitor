#include <iostream>
#include <fstream>
#include <sstream>

int main(){
    char line[256]={0};
    std::fstream fs;
    fs.open("/proc/net/tcp",std::fstream::in);
    fs.getline(line,256);
    while(fs.getline(line,256)){
        std::cout<<line<<std::endl;
    }
    fs.close();
}