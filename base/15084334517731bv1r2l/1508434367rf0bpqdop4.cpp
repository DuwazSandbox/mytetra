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
		File.init(this, &stream::get_file);
		IsOpen.init(this, &stream::get_isOpen);

		file = fopen(filename.c_str(), mode);
		if (file != NULL) isOpen = true;
	}

	void close()
	{
		if (isOpen)
		{
			fclose(file);
			isOpen = false;
		}
	}

	virtual ~stream()
	{
		if (isOpen)
			fclose(file);
	}

	bool get_isOpen() { return isOpen; }
	FILE* get_file() { return file; }

	property <FILE*, stream> File;
	property <bool, stream> IsOpen;

private:
	FILE* file;
	bool isOpen;
};

class ostream:public stream
{
public:
	ostream(std::string filename):stream(filename, "w") {}
	ostream operator << (std::string toOut)
	{
		try
		{
			fputs(toOut.c_str(), File);
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
		if (IsOpen)
		{
			Str.erase();
			try
			{
				char symbol;
				while ((symbol = fgetc(File)) > 0 && symbol != '\n')
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