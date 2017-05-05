#include "mainwindow.h"
#include <QApplication>

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP



int main(int argc, char *argv[])
{
    START_EASYLOGGINGPP(argc, argv);
    el::Configurations cfg("test_conf.conf");
    el::Loggers::reconfigureAllLoggers(cfg);
    el::Loggers::removeFlag(el::LoggingFlag::NewLineForContainer);



    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    LOG(INFO) << "First log";
//    LOG(INFO) << "Second log";
//    LOG(WARNING) << "Starting Qt Logging";
//    LOG(DEBUG) << "Printing qstack ";
//    VLOG(2) << "This is verbose level 2 logging from thread #";
//    LOG_EVERY_N(2, WARNING) << "This will be logged only once from thread who every reaches this line first. Currently running from thread #";
//    VLOG_EVERY_N(2, 3) << "Verbose level 3 log every two times. This is at  <<from thread #";
//    LOG_EVERY_N(1, INFO) << "This interval log will be logged with every thread, this one is from thread #";
//    LOG_IF(2 == 2, INFO) << "This log is only for thread 2 and is ran by thread #";

    return a.exec();
}
