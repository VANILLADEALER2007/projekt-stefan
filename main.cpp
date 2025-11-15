#include <iostream>
#include <ctime>
#include <string>
#include <list>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <sstream>

using namespace std;

//funkcja konwertująca typ danych time_t na czytelny ciąg znaków string
string formatujDate(time_t data)
{
	char buffer[20];
	tm *czas = localtime(&data);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d", czas);
	return string(buffer);
}

//konersja z string do time_t
time_t getTimeFromString(string timeString)
{
	tm tm = {};
	tm.tm_isdst = -1;
	istringstream iss(timeString);
	iss >> get_time(&tm, "%Y-%m-%d");
	time_t data;
	if (iss.fail())
	{
		cout << "Blad w trakcie proby odczytania daty!" << endl;
	}
	else
	{
		data = mktime(&tm);
	}
	return data;
}

//klasy
//klasa config
class Config
{
	private:
	static const string ConfigPath;
	static bool WczytanoConfig;
	//sprawdzamy czy plik config jest utworzony
	
	public:
	bool Dostepnosc()
	{
		struct stat buffer;
		return (stat(ConfigPath.c_str(), &buffer) == 0);
	}
	//tworzenie configu - za pierwszym razem
	void Utworz()
	{
		ofstream Config(ConfigPath);
		Config << "0";
		Config.close();
	}
	//wczytywanie configu
	void Wczytaj()
	{
		ifstream Config(ConfigPath);
		Config >> last_id;
        Config.close();
	}
	//zapis do pliku 
	void Zapisz()
	{
		ofstream Config(ConfigPath);
		Config << last_id;
		Config.close();
	}
	static unsigned int last_id;
	//konstruktor
	Config()
	{
		if (!WczytanoConfig)
		{
			if (!Dostepnosc())
			{
				cout << "Brak pliku konfiguracyjnego, tworze nowy..." << endl;
				Utworz();
			}
			else
			{
				cout << "Wczytywanie pliku konfiguracyjnego..." << endl;
				Wczytaj();
			}
			WczytanoConfig = true;
		}
	}
};

//statyczne pola do config
unsigned int Config::last_id = 0;
bool Config::WczytanoConfig = false;
const string Config::ConfigPath = filesystem::current_path().string() + "/Config.txt";

//klasa wpis
class Wpis
{
public:
    //te wartości będzie posiadać wpis
	unsigned int id;
	time_t data;
	double wartosc;
	string typ, kategoria, notatka;
	
	//konstruktor
	Wpis(string typ, double wartosc, string kategoria, string notatka)
	{
		this->id = ++Config::last_id;
		this->data = time(0);
		localtime(&data);
		this->typ = typ;
		this->wartosc = wartosc;
		this->kategoria = kategoria;
		this->notatka = notatka;
	}

	//konstruktor do odczytu z pliku (z danymi: id, data, typ, wartosc, kategoria, notatka)
	Wpis(unsigned int id, time_t data, string typ, double wartosc, string kategoria, string notatka)
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
		cout << "   Czas: " << formatujDate(data);
		cout << "   Typ: " << typ;
		cout << "   Wartosc: " << wartosc;
		cout << "   Kategoria: " << kategoria;
		cout << "   Notatka: " << notatka << endl;
	}

	void zapiszDoCSV(ofstream &plik)
	{
		plik << id << ","
			 << formatujDate(data) << ","
			 << typ << ","
			 << wartosc << ","
			 << kategoria << ","
			 << notatka << endl;
	}
};

//pola statyczne dla klasy wpis
list<Wpis> lista_wpisow;

//pobieranie typu double - jesli podamy np. string, program sie nie wywali
double GetDouble()
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

//poprawne pobieranie kategorii
string GetCategory()
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

//sortowanie listy po kwocie
void SortujListePoKwocie()
{
	list<Wpis> posortowanaLista;
	Wpis najwiekszaKwota = Wpis("", INT32_MAX, "", "");
	double najmniejszaRoznica = 0;
	for (int i = 0; i < lista_wpisow.size(); i++)
	{
		Wpis lastKwota = Wpis("", 0.0, "", "");
		for (Wpis w : lista_wpisow)
		{
			if(w.wartosc >= lastKwota.wartosc && w.wartosc <= najwiekszaKwota.wartosc && w.id != lastKwota.id && najwiekszaKwota.id != w.id)
			{
				lastKwota = w;
			}
		}
		najwiekszaKwota = lastKwota;
		posortowanaLista.push_front(lastKwota);
	}

	lista_wpisow = posortowanaLista;
}

int main()
{
	// inicjalizacja zmiennnych ktore beda przekazywane do klasy z programu glownego
	string typ_operacji, kategoria_operacji, notatka_operacji;
	double wartosc_operacji;
	bool dzialanie_programu = true;

	// inne zmienne
	int nr_operacji;

	Config config;

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
				Wpis wpis(typ_operacji, wartosc_operacji, kategoria_operacji, notatka_operacji);
				// dodawanie wpisu (tu: obiektu klasy) do listy wpisow
				lista_wpisow.push_front(wpis);
				break;
			}
		}
		case 2:
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
			break;
		}
			//	    case 3:
			//	    	break;
		case 4: // sortowanie (jak narazie tylko po kwocie, trzeba dodac pozostale sposoby sortowania, tj. data + id, kwota->ale malejąco, moze kategorami alfabetycznie ;P)
			SortujListePoKwocie();
			cout << "lista zostala posortowana po kwocie!" << endl;
			break;
			//	    case 5:
			//	    	break;
		case 6: // zapis
		{
			ofstream plik("Data.txt", ofstream::trunc);

			if (!plik.is_open())
			{
				cout << "Nie mozna otworzyc pliku do zapisu!" << endl;
				break;
			}

			for (Wpis &wpis : lista_wpisow)
			{
				wpis.zapiszDoCSV(plik);
			}

			plik.close();
			cout << "Zapisano dane do pliku Data.txt" << endl;
			config.Zapisz();
			break;
		}
		case 7: // odczyt
		{
			ifstream plik("Data.txt");

			if (!plik.is_open())
			{
				cout << "Nie mozna otworzyc pliku do odczytu!" << endl;
				break;
			}
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

				time_t data = getTimeFromString(data_str);

				double wartosc = stod(wartosc_str);

				Wpis wpis(id, data, typ, wartosc, kategoria, notatka);
				lista_wpisow.push_back(wpis); // można użyć push_back, żeby zachować kolejność z pliku

				// Uaktualnij last_id, jeśli potrzebne
				if (id > Config::last_id)
					Config::last_id = id;
			}

			plik.close();
			cout << "Dane zostaly wczytane z pliku Data.txt" << endl;
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
	return 0;
}
