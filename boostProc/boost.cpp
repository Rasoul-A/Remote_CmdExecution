// #include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <boost/process.hpp>
#include <boost/process/child.hpp>
namespace bp = boost::process;
// namespace bpo = boost::program_options;
// namespace bp = boost::process;
using namespace std;
void create_process()
{
    std::error_code ec;
    bp::ipstream err;
    bp::ipstream is;
    std::string cmd1 = "bash -c \"ls && sleep 1\"";
    bp::system(cmd1.c_str(), bp::std_err > err, bp::std_out > is);
    std::vector<std::string> er, in;
    std::string line, output , error;
    while (std::getline(err, line) && !line.empty())
    {
        er.push_back(line + ",");
    }
    line.clear();
    while (std::getline(is, line) && !line.empty())
    {
        in.push_back(line + ",");
    }
    for (const auto &piece : in) output += piece;
    for (const auto &piece : er) error += piece;
    cout<<"output :"<<output<<endl;
    cout<<"error :"<<error<<endl;
}

int main()
{
    while (true)
    {
        
    
    // sleep(1);
    create_process();

    }
    // using namespace boost::lambda;
    // typedef std::istream_iterator<int> in;

    // std::for_each(
    //     in(std::cin), in(), std::cout << (_1 * 3) << " " );
}
