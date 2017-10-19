#include <cstdio>
#include <string>
#include <iostream>
#include <exception>
#include <cassert>

template <typename proptype, typename propowner>
class property
{
private:
	typedef proptype (propowner::*getter)();
	typedef void (propowner::*setter)(proptype);
	propowner * m_owner;
	getter m_getter;
	setter m_setter;

public:
	// Оператор приведения типа. Реализует свойство для чтения.
	operator proptype()
	{
		// Здесь может быть проверка "m_owner" и "m_getter" на NULL
		return (m_owner->*m_getter)();
	}
	// Оператор присваивания. Реализует свойство для записи.
	void operator =(proptype data)
	{
		// Проверка "m_owner" и "m_setter" на NULL
		assert(m_owner != NULL && m_setter != NULL);
		(m_owner->*m_setter)(data);
	}
	// Конструктор по умолчанию.
	property() :
		m_owner(NULL),
		m_getter(NULL),
		m_setter(NULL)
	{
	}
	//Конструктор инициализации.
	property(propowner * const owner, getter getmethod, setter setmethod) :
		m_owner(owner),
		m_getter(getmethod),
		m_setter(setmethod)
	{
	}
	// Инициализация
	void init(propowner * const owner, getter getmethod, setter setmethod = NULL)
	{
		m_owner = owner;
		m_getter = getmethod;
		m_setter = setmethod;
	}
};

class stream
{
public:
	stream(std::string filename, const char* mode)
	{
		file.init(this, &stream::get_file);
		isOpen.init(this, &stream::get_isOpen);

		l_file = fopen(filename.c_str(), mode);
		if (l_file != NULL) l_isOpen = true;
	}

	void close()
	{
		if (l_isOpen)
		{
			fclose(l_file);
			l_isOpen = false;
		}
	}

	virtual ~stream()
	{
		if (l_isOpen)
			fclose(l_file);
	}

	bool get_isOpen() { return l_isOpen; }
	FILE* get_file() { return l_file; }

	property <FILE*, stream> file;
	property <bool, stream> isOpen;

private:
	FILE* l_file;
	bool l_isOpen;
};

class ostream:public stream
{
public:
	ostream(std::string filename):stream(filename, "w") {}
	ostream operator << (std::string toOut)
	{
		try
		{
			fputs(toOut.c_str(), file);
		}
		catch (std::exception ex)
		{
			std::cerr << ex.what();
		}
		return *this;
	}
};

class istream:public stream
{
public:
	istream(std::string filename):stream(filename, "r") {};

	istream& operator>> (std::string& Str)
	{
		if (isOpen)
		{
			Str.erase();
			try
			{
				char symbol;
				while ((symbol = fgetc(file)) > 0 && symbol != '\n')
					Str.append(1, symbol);
			}
			catch (std::exception ex)
			{
				std::cerr << ex.what();
			}
		}
		return *this;
	}
};

int main()
{
	std::string first("conca");
	std::string second("ti\nation");
	ostream output("file.txt");
	output << first << second;
	output.close();

	std::string third, fourth;
	istream input("file.txt");
	input >> third >> fourth;

	std::cout << fourth << std::endl;
	return 0;
}