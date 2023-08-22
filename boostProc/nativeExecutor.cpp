#include <iostream>
#include <iterator>
#include <algorithm>
#include <fcntl.h>              /* Definition of O_* constants */
       #include <unistd.h>
using namespace std;
int exec(const char * command, string *input, string *output, string *error)
{
    int pfd[2];
    FILE *pout ,*perr;
    // int pout = -1;
    if (pipe(pfd) < 0)
    return -1;
    
perr = fdopen(pfd[0], "r");
// string fcmd = "bash -c \"ls && sleep 1 \" 2>&" +to_string(pfd[1]);
string fcmd = std::string("bash -c \'"+std::string(command)+"\' 2>&"+to_string(pfd[1]));
// snprintf(command.c_str(), LINE_LEN, "... %d", );
char buffer[128];
string cmdout = "";
string cmderr = "";
if (pout = popen(fcmd.c_str(), "r")) {
    close(pfd[1]);
while (!feof(pout))
    {
        if (fgets(buffer, 128, pout) != NULL)
        cmdout += buffer;
    }
    // memset();
while (!feof(pout))
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
return 0;
}


int main()
{
    while (true)
    {
        
        string err , out;
    exec((char*)"cat xxxxxx", nullptr, &out , &err);
    cout<<"CMD out: "<<out<<endl;
    cout<<"CMD err: "<<err<<endl;
    sleep(1);

    }
    // using namespace boost::lambda;
    // typedef std::istream_iterator<int> in;

    // std::for_each(
    //     in(std::cin), in(), std::cout << (_1 * 3) << " " );
}