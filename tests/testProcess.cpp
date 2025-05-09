#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <list>
#include <string>

#include <sm/process>

/*
 * Callbacks class extends ProcessCallbacks
 */
class TestProcessCallbacks : public sm::process_callbacks
{
public:
    TestProcessCallbacks (sm::process_data* p) { this->parent = p; }
    void startedSignal (std::string msg) { std::cout << "Started. msg: " << msg << std::endl;}
    void errorSignal (int err) { this->parent->setErrorNum (err); }
    void processFinishedSignal (std::string msg) { this->parent->setProcessFinishedMsg (msg); }
    void readyReadStandardOutputSignal() { this->parent->setStdOutReady (true); }
    void readyReadStandardErrorSignal() { this->parent->setStdErrReady (true); }
private:
    sm::process_data* parent;
};

int main()
{
    int rtn = 0;

    // Set up command and args
    std::list<std::string> args;
    args.push_back ("git");
    args.push_back ("status");
    args.push_back ("2>/dev/null");
    std::string command ("/usr/bin/git");

    // Create a ProcessData object to capture output from the program
    sm::process_data pD;
    // Create a callbacks class that'll manage capturing the data from the program
    TestProcessCallbacks cb(&pD);
    // Instantiate the Process object
    sm::process p;

    try {
        // set its callbacks
        p.setCallbacks (&cb);
        // and start it
        p.start (command, args);
        p.probeProcess ();
        if (!p.waitForStarted()) {
            throw std::runtime_error ("Process failed to start");
        } else {
            std::cout << "Process started successfully" << std::endl;
        }
        while (p.running() == true) {
            p.probeProcess();
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        rtn = -1;
    }

    // Do something with the output (print to screen)
    std::stringstream theOutput;
    theOutput << p.readAllStandardOutput();
    std::string line = "";
    std::cout << "A call to `git status` gives the following output:\n"
              << "--------------------------------------------------\n";
    while (getline (theOutput, line, '\n')) {
        std::cout << line << std::endl;
    }

    return rtn;
}
