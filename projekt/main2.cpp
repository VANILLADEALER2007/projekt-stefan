#include <iostream>
#include <ctime>

using namespace std;
class wpis{
	public:
	int id;
	time_t data;
	double wartosc;
	string kategoria;
	string notatka;
	enum Typ
	{
		Wydatek,
		Przychod
	};
};
