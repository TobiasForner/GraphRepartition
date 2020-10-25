//
// Created by Tobias on 19/11/2019.
//

#include "IterationTimeWriter.h"

IterationTimeWriter::IterationTimeWriter(std::string filename, int intervalSize) : writer(CLSWriter(filename)),
                                                                                   intervalSize(intervalSize) {
    endOfInterval = intervalSize - 1;
    writer.writeToFile("Interval,time (ms)");
    timePassed = std::chrono::microseconds(0);
    totalTimePassed = std::chrono::microseconds(0);
}

void IterationTimeWriter::startIterationTiming(int iteration) {
    start = std::chrono::high_resolution_clock::now();
}

void IterationTimeWriter::stopIterationTiming(int iteration) {
    end = std::chrono::high_resolution_clock::now();
    timePassed = timePassed + std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    totalTimePassed = totalTimePassed + std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    start = std::chrono::high_resolution_clock::now();
    if (iteration == endOfInterval) {
        writer.writeToFile(
                std::to_string(endOfInterval - intervalSize + 1) + "-" + std::to_string(endOfInterval) + "," +
                std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(totalTimePassed).count()));
        endOfInterval += intervalSize;
        timePassed = std::chrono::microseconds(0);
    }
}

void IterationTimeWriter::addConfig(std::vector<std::pair<std::string, std::string>> c) {
    writer.writeToFile("Configuration");
    for (auto pair:c) {
        writer.writeToFile(pair.first + "," + pair.second);
    }
    writer.writeToFile("");
}

void IterationTimeWriter::shutdown() {
    writer.closeFile();
}

void IterationTimeWriter::printData() {
    std::cout << "Total computation time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(totalTimePassed).count() << " ms\n";
}
