#include <iostream>
#include <iterator>
#include <algorithm>
#include <fcntl.h> /* Definition of O_* constants */
#include <unistd.h>
#include <thread>
using namespace std;
void exec(const char *command, string *input, string *output, string *error)
{
    int pfd[2];
    FILE *pout, *perr;
    // int pout = -1;
    if (pipe(pfd) < 0)
        // return -1;
        printf("error!\n");

    perr = fdopen(pfd[0], "r");
    // string fcmd = "bash -c \"ls && sleep 1 \" 2>&" +to_string(pfd[1]);
    string fcmd = std::string("bash -c \'" + std::string(command) + "\' 2>&" + to_string(pfd[1]));
    std::cout<<"##############Prepared CMD: "<<fcmd.c_str()<<"\n";
    // snprintf(command.c_str(), LINE_LEN, "... %d", );
    char buffer[128];
    string cmdout = "";
    string cmderr = "";
    if (pout = popen(fcmd.c_str(), "r"))
    {
        close(pfd[1]);
        while (!feof(pout))
        {
            if (fgets(buffer, 128, pout) != NULL)
                cmdout += buffer;
        }
        // memset();
        while (!feof(perr))
        {
            if (fgets(buffer, 128, perr) != NULL)
                cmderr += buffer;
        }

        pclose(pout);
    }

    fclose(perr);
    close(pfd[0]), close(pfd[1]);
    *output = cmdout;
    *error = cmderr;
    // return 0;
}

int main()
{
    while (true)
    {

        string err, out;
        // exec((char *)"ls && sleep 1 && cat xxxxxx", nullptr, &out, &err);
        std::thread th1(exec, (char *)"ls && sleep 1 && cat xxxxxx", nullptr, &out, &err);
        string err2, out2;
        // exec((char *)"ls && sleep 1 && cat xxxxxx", nullptr, &out, &err);
        std::thread th2(exec, (char *)"ls && sleep 1 && cat xxxxxx", nullptr, &out2, &err2);
        th1.join();
        th2.join();
        cout << "CMD out: " << out << endl;
        cout << "CMD err: " << err << endl;
        cout << "CMD out2: " << out2 << endl;
        cout << "CMD err2: " << err2 << endl;
        sleep(1);
    }
    // using namespace boost::lambda;
    // typedef std::istream_iterator<int> in;

    // std::for_each(
    //     in(std::cin), in(), std::cout << (_1 * 3) << " " );
}