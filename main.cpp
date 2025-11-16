#include <iostream>
#include <ctime>
#include <string>
#include <list>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <sstream>

using namespace std;

class DataTime
{
private:
	string strTime;
	time_t tTime;

public:
	DataTime(bool CurrentTime)
	{
		if (CurrentTime)
		{
			tTime = time(0);
			localtime(&tTime);
			DataTime a = DataTime(tTime);
			strTime = a.GetString();
		}
		else
		{
			tTime = time(0);
			DataTime a = DataTime(tTime);
			strTime = a.GetString();
		}
	}

	DataTime(time_t data)
	{
		tTime = data;
		char buffer[20];
		tm *czas = localtime(&data);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d", czas);
		strTime = string(buffer);
	}

	DataTime(string timeString)
	{
		strTime = timeString;
		tm tm = {};
		tm.tm_isdst = -1;
		istringstream iss(timeString);
		iss >> get_time(&tm, "%Y-%m-%d");
		if (iss.fail())
		{
			cout << "Blad w trakcie proby odczytania daty!" << endl;
		}
		else
		{
			tTime = mktime(&tm);
		}
	}

	string GetString()
	{
		return strTime;
	}

	time_t GetTime_T()
	{
		return tTime;
	}
};

// klasa wpis
class Wpis
{
public:
	// te wartości będzie posiadać wpis
	unsigned int id;
	DataTime data = DataTime(true);
	double wartosc;
	string typ, kategoria, notatka;

	// konstruktor - tworzenie nowego wpisu o aktualnej dacie
	Wpis(int id, string typ, double wartosc, string kategoria, string notatka)
	{
		this->id = id;
		this->typ = typ;
		this->wartosc = wartosc;
		this->kategoria = kategoria;
		this->notatka = notatka;
	}

	// konstruktor do odczytu z pliku (z danymi: id, data, typ, wartosc, kategoria, notatka)
	Wpis(unsigned int id, DataTime data, string typ, double wartosc, string kategoria, string notatka)
	{
		this->id = id;
		this->data = data;
		this->typ = typ;
		this->wartosc = wartosc;
		this->kategoria = kategoria;
		this->notatka = notatka;
	}

	void wyswietl()
	{
		cout << "ID: " << id;
		cout << "   Czas: " << data.GetString();
		cout << "   Typ: " << typ;
		cout << "   Wartosc: " << wartosc;
		cout << "   Kategoria: " << kategoria;
		cout << "   Notatka: " << notatka << endl;
	}

	void zapiszDoCSV(ofstream &plik)
	{
		plik << id << ","
			 << data.GetString() << ","
			 << typ << ","
			 << wartosc << ","
			 << kategoria << ","
			 << notatka << endl;
	}
};

list<Wpis> lista_wpisow;
int last_id;

class UserMenu
{
	// pobieranie typu double - jesli podamy np. string, program sie nie wywali
	static double GetDouble()
	{
		double x;
		cin >> x;
		while (cin.fail() || (cin.peek() != '\r' && cin.peek() != '\n'))
		{
			cout << "Nieprawidłowa liczba! Wprowadź liczbę z przecinkiem używając '.' zamiast przecinka: ";
			cin.clear();
			while (cin.get() != '\n')
				;
			cin >> x;
		}
		return x;
	}

	// poprawne pobieranie kategorii
	static string GetCategory()
	{
		bool good = false;
		string kategoria_operacji;
		while (!good)
		{
			getline(cin, kategoria_operacji);
			size_t found = kategoria_operacji.find(",");

			if (found != string::npos)
			{
				cout << "Nie mozesz uzyc znaku ',' w kategorii, podaj kategorie nie uzywajac przecinka: ";
			}
			else
			{
				good = true;
			}
		}
		return kategoria_operacji;
	}

	static void Zapisz()
	{
		ofstream plik("Data.txt", ofstream::trunc);

		if (!plik.is_open())
		{
			cout << "Nie mozna otworzyc pliku do zapisu!" << endl;
		}
		else
		{

			for (Wpis &wpis : lista_wpisow)
			{
				wpis.zapiszDoCSV(plik);
			}

			plik.close();
			cout << "Zapisano dane do pliku Data.txt" << endl;
		}
	}

	static void Wczytaj()
	{
		ifstream plik("Data.txt");

		if (!plik.is_open())
		{
			cout << "Nie mozna otworzyc pliku do odczytu!" << endl;
		}
		else
		{
			lista_wpisow.clear();
			string linia;
			while (getline(plik, linia))
			{
				// Zakładamy format CSV: id,data,typ,wartosc,kategoria,notatka
				stringstream ss(linia);
				string id_str, data_str, typ, wartosc_str, kategoria, notatka;

				getline(ss, id_str, ',');
				getline(ss, data_str, ',');
				getline(ss, typ, ',');
				getline(ss, wartosc_str, ',');
				getline(ss, kategoria, ',');
				getline(ss, notatka);

				unsigned int id = stoi(id_str);

				DataTime data = DataTime(data_str);

				double wartosc = stod(wartosc_str);

				Wpis wpis(id, data, typ, wartosc, kategoria, notatka);
				lista_wpisow.push_back(wpis); // można użyć push_back, żeby zachować kolejność z pliku
			}
			plik.close();

			for (Wpis a : lista_wpisow)
			{
				if (last_id < a.id)
				{
					last_id = a.id;
				}
			}
			cout << "Dane zostaly wczytane z pliku Data.txt" << endl;
		}
	}

	static void Wyswietl()
	{
		if (!lista_wpisow.empty())
		{
			for (Wpis wpis : lista_wpisow)
			{
				wpis.wyswietl();
			}
		}
		else
		{
			cout << "Lista jest pusta! Wczytaj wczesniej utworzona liste za pomoca opcji '7. Odczyt' lub zacznij dodawac wpisy za pomoca '1. Dodaj'." << endl;
		}
	}

	static void DodawanieWpisu()
	{
		string typ_operacji, kategoria_operacji, notatka_operacji;
		double wartosc_operacji;
		// typ operacji
		bool wyborTypu = true;
		// weryfikacja poprawnosci wpisanego typu operacji
		while (wyborTypu)
		{
			cout << "Podaj typ operacji (przychod/wydatek/anuluj): ";
			cin >> typ_operacji;
			if (typ_operacji == "przychod" || typ_operacji == "wydatek")
				wyborTypu = false;
			else if (typ_operacji == "anuluj")
				wyborTypu = false;
			else
				cout << "Niepoprawny typ operacji" << endl;
		}

		// jesli nie anulowano
		if (typ_operacji != "anuluj")
		{
			// kwota operacji
			cout << "Podaj wartosc (kwote) operacji: ";

			wartosc_operacji = GetDouble();
			// kategoria operacji
			cout << "Podaj kategorie operacji: ";
			// kat operacji - odpowiedzialnosc po stronie uzytkownika (w razie literowek uzytkownik nie stosuje 6. Zapis w celu zapisania do pliku)
			cin.ignore(); // czyści bufor wejściowy z pozostałości po poprzednim cin

			kategoria_operacji = GetCategory();

			// notatka do operacji
			cout << "Podaj notatke do operacji: ";
			getline(cin, notatka_operacji);
			// dodawanie wpisu do klasy
			last_id++;
			Wpis wpis(last_id, typ_operacji, wartosc_operacji, kategoria_operacji, notatka_operacji);
			// dodawanie wpisu (tu: obiektu klasy) do listy wpisow
			lista_wpisow.push_front(wpis);
		}
	}

	public:
	static void Start()
	{
		bool dzialanie_programu = true;
		int nr_operacji;

		cout << fixed << std::setprecision(2);

		while (dzialanie_programu)
		{
			cout << "Witamy w menedzerze budzetu domowego, prosimy wybrac operacje wpisujac cyfre od 1 do 8:" << endl;
			cout << "1. Dodaj   2. Lista   3. Filtruj   4. Sortuj   5. Statystyki   6. Zapis   7. Odczyt   8. Wyjscie" << endl;
			cin >> nr_operacji;
			switch (nr_operacji)
			{
			case 1:
			{
				DodawanieWpisu();
			}
			case 2:
			{
				Wyswietl();
				break;
			}
			case 6: // zapis
			{
				Zapisz();
				break;
			}
			case 7: // odczyt
			{
				Wczytaj();
				break;
			}
			case 8:
			{
				cout << "Zamykam program, dziekujemy za skorzystanie z naszej uslugi! :)" << endl;
				dzialanie_programu = false;
				break;
			}
			default:
				cout << "Wybrano niewlasciwy numer" << endl;
				break;
			}
		}
	}
};

int main()
{
	UserMenu::Start();
	return 0;
}
