#include <string>
#include <iostream>

using std::string;

int main()
{
	string str = "sunguanke";
    
	std::cout << sizeof(str)  << " " << sizeof(str.c_str())<< std::endl;
	return 0;
}
	
