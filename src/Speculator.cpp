#include "Speculator.h"
#include <iostream>

Speculator::Speculator() {}

void Speculator::create_speculation(){
    std::cout<<"Create Speculation!"<<std::endl;
}

void Speculator::commit_speculation(){
    std::cout<<"Commit Speculation!"<<std::endl;
}

void Speculator::fail_speculation(){
    std::cout<<"Fail Speculation!"<<std::endl;
}