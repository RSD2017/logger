#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

class logger {

    public:
        logger();
        ~logger();

        void log_order_ok(std::string ID, bool good, std::vector<std::string> part_ID );
        void log_order_ng( std::string ID, bool good, std::vector<std::string> part_ID, std::string error_msg );
        void log_additonal_error( std::string error_msg );

    private:
        std::string log_file_name = "./log/log_file.txt";
        std::string error_file_name = "./log/error_file.txt";

        time_t timer;
        struct tm *Tm;
        std::ofstream log_file;
        std::ofstream error_file;
        std::string order = "-";

        std::string get_time();
};

#endif // LOGGER_H
