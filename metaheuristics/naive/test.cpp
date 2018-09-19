#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

int main()
{
    std::string str = "    This                    is a      string    ";
    std::istringstream buf(str);
    std::istream_iterator<std::string> beg(buf), end;

    std::vector<std::string> tokens(beg, end); // done!

    for(auto& s: tokens)
        std::cout << '"' << s << '"' << '\n';
}