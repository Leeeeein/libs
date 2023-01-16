#include <iostream>
#include <fstream>
 
std::ofstream os ("Brainfuck.txt");
std::string str = "Hello, world";
 
void print (char ch)
{
	for (int i = 1; i <= ch; ++i)
	{
		os << '+';
	}
	os << ".>" << std::endl;
}
 
int main ()
{
	std::cin >> str;
	for (char ch : str)
	{
		print (ch);
	}
	return 0;
}

