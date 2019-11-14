#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <codecvt>
#include <vector>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace alt
{
	class Log : protected std::ostream
	{
	public:
		enum Color
		{
			BLACK, LBLACK,
			RED, LRED,
			GREEN, LGREEN,
			BLUE, LBLUE,
			YELLOW, LYELLOW,
			MAGENTA, LMAGENTA,
			CYAN, LCYAN,
			WHITE, LWHITE
		};

		using StdStreamManip = std::ostream&(*)(std::ostream&);
		using LogManip = Log & (*)(Log&);

		Log& Write(const std::string& val)
		{
			for (auto& s : streams)
				s->Put(val);

			return *this;
		}

		template<class T>
		Log& Put(const T& val)
		{
			(*static_cast<std::ostream*>(this)) << val;
			return *this;
		}

		Log& Put(bool val)
		{
			return Put(val ? "true" : "false");
		}

		Log& Put(LogManip val)
		{
			return val(*this);
		}

		Log& Put(StdStreamManip val)
		{
			(*static_cast<std::ostream*>(this)) << val;
			return *this;
		}

		template<class... Args>
		Log& Put(LogManip val, const Args&... args)
		{
			return Put(val).Put(args...);
		}

		template<class T, class... Args>
		Log& Put(const T& val, const Args&... args)
		{
			return Put(val).Put(" ").Put(args...);
		}

		Log& PutTime()
		{
			const time_t t = time(nullptr);
			(*static_cast<std::ostream*>(this)) << std::put_time(localtime(&t), "[%H:%M:%S]") << std::flush;
			return *this;
		}

		Log& PutColor(Color col)
		{
			*this << std::flush;

			for (auto& s : streams)
				s->PutColor(col);

			return *this;
		}

		template<class T> Log& operator<<(const T& val) { return Put(val); }
		Log& operator<<(StdStreamManip val) { return Put(val); }
		Log& operator<<(LogManip val) { return Put(val); }

		// Manipulators
		static Log& Black(Log& log) { return log.PutColor(BLACK); }
		static Log& LBlack(Log& log) { return log.PutColor(LBLACK); }
		static Log& Red(Log& log) { return log.PutColor(RED); }
		static Log& LRed(Log& log) { return log.PutColor(LRED); }
		static Log& Green(Log& log) { return log.PutColor(GREEN); }
		static Log& LGreen(Log& log) { return log.PutColor(LGREEN); }
		static Log& Blue(Log& log) { return log.PutColor(BLUE); }
		static Log& LBlue(Log& log) { return log.PutColor(LBLUE); }
		static Log& Yellow(Log& log) { return log.PutColor(YELLOW); }
		static Log& LYellow(Log& log) { return log.PutColor(LYELLOW); }
		static Log& Magenta(Log& log) { return log.PutColor(MAGENTA); }
		static Log& LMagenta(Log& log) { return log.PutColor(LMAGENTA); }
		static Log& Cyan(Log& log) { return log.PutColor(CYAN); }
		static Log& LCyan(Log& log) { return log.PutColor(LCYAN); }
		static Log& White(Log& log) { return log.PutColor(WHITE); }
		static Log& LWhite(Log& log) { return log.PutColor(LWHITE); }
		static Log& Endl(Log& log) { return log.Put(std::endl).Put(std::dec).PutColor(WHITE); }
		static Log& Time(Log& log) { return log.PutTime(); }

		struct Log_Base
		{
			virtual Log& Begin() const = 0;
			template<class... Args> Log& operator()(const Args&... args) const { return Begin().Put<Args...>(args...).Put(Endl); }
			template<class T> Log& operator<<(const T& val) const { return Begin().Put<T>(val); }
		};

#if __cpp_constexpr >= 201603L
		static constexpr struct Log_Raw : public Log_Base {
			Log& Begin() const override { return Instance(); }
		} Raw{};

		static constexpr struct Log_Info : public Log_Base {
			Log& Begin() const override { return Instance().Put(White, Time, " "); }
		} Info{};

		static constexpr struct Log_Warning : public Log_Base {
			Log& Begin() const override { return Instance().Put(LYellow, Time, "[Warning] "); }
		} Warning{};

		static constexpr struct Log_Error : public Log_Base {
			Log& Begin() const override { return Instance().Put(Red, Time, "[Error] "); }
		} Error{};

#ifndef NDEBUG
		static constexpr struct Log_Debug : public Log_Base {
		private:
			Log& Begin() const override { return Instance().Put(Yellow, Time, "[Debug] "); }
		} Debug{};
#else
		static constexpr struct Log_Debug {
			const Log_Debug& Begin() const { return *this; }
			template<class T> const Log_Debug& operator<<(T) const { return *this; }
			const Log_Debug& operator<<(StdStreamManip) const { return *this; }
			template<class... Args> const Log_Debug& operator()(const Args&... args) const { return *this; }
		} Debug{};
#endif // NDEBUG

#define ALT_LOG_IMPL

#else
		static struct Log_Raw : public Log_Base {
			Log& Begin() const override { return Instance(); }
		} Raw;

		static struct Log_Info : public Log_Base {
			Log& Begin() const override { return Instance().Put(White, Time, " "); }
		} Info;

		static struct Log_Warning : public Log_Base {
			Log& Begin() const override { return Instance().Put(LYellow, Time, "[Warning] "); }
		} Warning;

		static struct Log_Error : public Log_Base {
			Log& Begin() const override { return Instance().Put(Red, Time, "[Error] "); }
		} Error;

#ifndef NDEBUG
		static struct Log_Debug : public Log_Base {
		private:
			Log& Begin() const override { return Instance().Put(Yellow, Time, "[Debug] "); }
		} Debug;
#else
		static struct Log_Debug {
		private:
			const Log_Debug& Begin() const { return *this; }

		public:
			template<class T> const Log_Debug& operator<<(T) const { return *this; }
			const Log_Debug& operator<<(StdStreamManip) const { return *this; }
			template<class... Args> const Log_Debug& operator()(const Args&... args) const { return *this; }
		} Debug;
#endif // NDEBUG

#define ALT_LOG_IMPL \
	::alt::Log::Log_Raw alt::Log::Raw; \
	::alt::Log::Log_Info alt::Log::Info; \
	::alt::Log::Log_Warning alt::Log::Warning; \
	::alt::Log::Log_Error alt::Log::Error; \
	::alt::Log::Log_Debug alt::Log::Debug;

#endif // __cpp_constexpr

		static Log& Instance() noexcept
		{
			static Log s;
			return s;
		}

		class Stream
		{
		public:
			virtual Stream& Put(const std::string& val) = 0;
			virtual Stream& PutColor(Color color) = 0;
		};

		void AddOut(Stream* stream) { streams.emplace_back(stream); }

		static void Push(Stream* stream) { Instance().AddOut(stream); }

		class ConsoleStream : public Stream
		{
		public:
			Stream& Put(const std::string& val) override
			{
#ifdef _WIN32
				static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				WriteConsoleA(hConsole, val.data(), (DWORD)val.size(), NULL, NULL);
#else
				std::cout << val;
#endif // _WIN32

				return *this;
			}

			Stream& PutColor(Color val) override
			{
#ifdef _WIN32
				WORD col = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

				switch (val)
				{
				case BLACK:
					col = 0;
					break;
				case LBLACK:
					col = FOREGROUND_INTENSITY;
					break;
				case RED:
					col = FOREGROUND_RED;
					break;
				case LRED:
					col = FOREGROUND_RED | FOREGROUND_INTENSITY;
					break;
				case GREEN:
					col = FOREGROUND_GREEN;
					break;
				case LGREEN:
					col = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					break;
				case BLUE:
					col = FOREGROUND_BLUE;
					break;
				case LBLUE:
					col = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					break;
				case YELLOW:
					col = FOREGROUND_RED | FOREGROUND_GREEN;
					break;
				case LYELLOW:
					col = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					break;
				case MAGENTA:
					col = FOREGROUND_RED | FOREGROUND_BLUE;
					break;
				case LMAGENTA:
					col = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					break;
				case CYAN:
					col = FOREGROUND_GREEN | FOREGROUND_BLUE;
					break;
				case LCYAN:
					col = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					break;
				case WHITE:
					col = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
					break;
				case LWHITE:
					col = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					break;
				}

				static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, col);
#else
				switch (val)
				{
				case BLACK:
					std::cout << "\033[30m";
					break;
				case LBLACK:
					std::cout << "\033[90m";
					break;
				case RED:
					std::cout << "\033[31m";
					break;
				case LRED:
					std::cout << "\033[91m";
					break;
				case GREEN:
					std::cout << "\033[32m";
					break;
				case LGREEN:
					std::cout << "\033[92m";
					break;
				case BLUE:
					std::cout << "\033[34m";
					break;
				case LBLUE:
					std::cout << "\033[94m";
					break;
				case YELLOW:
					std::cout << "\033[33m";
					break;
				case LYELLOW:
					std::cout << "\033[93m";
					break;
				case MAGENTA:
					std::cout << "\033[35m";
					break;
				case LMAGENTA:
					std::cout << "\033[95m";
					break;
				case CYAN:
					std::cout << "\033[36m";
					break;
				case LCYAN:
					std::cout << "\033[96m";
					break;
				case WHITE:
					std::cout << "\033[37m";
					break;
				case LWHITE:
					std::cout << "\033[97m";
					break;
				}
#endif // _WIN32

				return *this;
			}

		};

		class FileStream : public Stream
		{
			std::ofstream file;
		public:
			FileStream(const std::string& fileName) : file(fileName) { }
#if _MSC_VER
			FileStream(const std::wstring& fileName) : file(fileName) { }
#endif

			Stream& Put(const std::string& val) override
			{
				file << val << std::flush;
				return *this;
			}

			Stream& PutColor(Color val) override { return *this; }
		};

	private:
		class Buffer : public std::streambuf
		{
			static const std::size_t BUF_SIZE = 1024;

			char buf[BUF_SIZE];

		public:
			using Traits = std::char_traits<char>;

			Buffer() { setp(buf, buf + BUF_SIZE); }

		protected:
			virtual Traits::int_type overflow(Traits::int_type c = Traits::eof()) override
			{
				put(pbase(), pptr());
				if (c != Traits::eof()) {
					char c2 = c;
					put(&c2, &c2 + 1);
				}
				setp(buf, buf + BUF_SIZE);

				return c;
			}

			virtual int sync() override
			{
				put(pbase(), pptr());
				setp(buf, buf + BUF_SIZE);
				return 0;
			}

		private:
			void put(const char* begin, const char* end) { Log::Instance().Write(std::string(begin, end)); }
		};

		Log() :std::ostream(&buf) { };
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;

		Buffer buf;
		std::vector<std::unique_ptr<Stream>> streams;
	};
}