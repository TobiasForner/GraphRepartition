//
// Created by Tobias on 19/11/2019.
//
#include <string>
#include <chrono>
#include <vector>
#include "../output/CLSWriter.h"

#ifndef DBGTTHESIS_ITERATIONTIMEWRITER_H
#define DBGTTHESIS_ITERATIONTIMEWRITER_H


class IterationTimeWriter {
public:
    IterationTimeWriter(std::string filename, int intervalSize);

    void startIterationTiming(int iteration);

    void stopIterationTiming(int iteration);

    void addConfig(std::vector<std::pair<std::string, std::string>> c);

    void shutdown();

    void printData();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    int intervalSize;
    int endOfInterval;
    CLSWriter writer;
    std::chrono::microseconds timePassed;
    std::chrono::microseconds totalTimePassed;
};


#endif //DBGTTHESIS_ITERATIONTIMEWRITER_H
