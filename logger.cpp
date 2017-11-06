#include "logger.h"

logger::logger()
{
    // Open
    log_file.open( log_file_name, std::ofstream::out | std::ofstream::app  );
    log_file << "LOG FILE " << get_time() << "\n";
    log_file << "time,order_id,type,parts_id\n";
    log_file.close();

    error_file.open( error_file_name, std::ofstream::out | std::ofstream::app  );
    error_file << "ERROR FILE " << get_time() << "\n";
    error_file << "time,error_msg\n";
    error_file.close();
}

void logger::log_order_ok( std::string ID, bool good, std::vector<std::string> part_ID )
{
    log_file.open( log_file_name, std::ofstream::out | std::ofstream::app );
    log_file << get_time() << "," << ID << "," << good;
    for(auto &part : part_ID) {
        log_file << "," << part;
    }
    log_file << "\n";
    log_file.close();
}

void logger::log_order_ng( std::string ID, bool good, std::vector<std::string> part_ID, std::string error_msg )
{
    log_file.open( log_file_name, std::ofstream::out | std::ofstream::app );
    log_file << get_time() << "," << ID << "," << good;
    for(auto &part : part_ID) {
        log_file << "," << part;
    }
    log_file << "," << error_msg << "\n";
    log_file.close();
}

void logger::log_additonal_error( std::string error_msg )
{
    error_file.open( error_file_name, std::ofstream::out | std::ofstream::app );
    error_file << get_time() << "," << error_msg << "\n";
    error_file.close();
}

std::string logger::get_time()
{
    // Timers
    timer=time(NULL);
    Tm=localtime(&timer);

    std::string ostime = std::to_string(Tm->tm_hour) + ":" +
                       std::to_string(Tm->tm_min) + ":" +
                       std::to_string(Tm->tm_sec);
    return ostime;
}
