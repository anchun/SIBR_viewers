#pragma once

# include <string>
# include <vector>
# include <map>

//temp replacement of sibr::Vector
# include <array>
# include "Config.hpp"
# include <core/system/Vector.hpp>

namespace sibr
{

	/// uint contexpr helper, defining the number of command line tokens required to init T
	template<typename T>
	constexpr uint NumberOfArg = 1;

	template<>
	constexpr uint NumberOfArg<bool> = 0;

	template<typename T, uint N>
	constexpr uint NumberOfArg<std::array<T, N>> = N * NumberOfArg<T>;

	/// Getter helper, return the n-th T from vector of strings
	template<typename T> struct ValueGetter {
		static T get(const std::vector<std::string> & values, uint n);
	};

	enum RenderingModes {
		RENDERMODE_MONO,
		RENDERMODE_STEREO_ANAGLYPH,
		RENDERMODE_STEREO_QUADBUFFER
	};

	/// Stores the command line parsed arguments
	/// only a static instance exists, that must be init with parseMainArgs(argc,argv) right after main(argc,argv)
	/// parses --key or --key with any number of value
	class SIBR_SYSTEM_EXPORT CommandLineArgs {

	public:
		static void parseMainArgs(const int argc, const char* const* argv);

		//get the Nth parsed element following -key or --key as a T 
		//if not available (key not found or not enough token), return default_val argument
		template<typename T, uint N = 0>
		T get(const std::string & key, const T & default_val) const {
			T out;
			if (getInternal<T,N>(key, out)) {
				return out;
			} else {
				return default_val;
			}
		}

		template<typename T, uint N = 0>
		T getRequired(const std::string & key) const {
			T out;
			if (!getInternal(key, out)) {
				SIBR_ERR;
			}
			return out;
		}

		bool contains(const std::string & key) const;
		int numArguments(const std::string & key) const;

		static const CommandLineArgs & getGlobal();

	protected:
		CommandLineArgs() {};

		template<typename T, uint N = 0>
		bool getInternal(const std::string & key, T & val) const {
			if (contains(key) && (N + 1)*NumberOfArg<T> <= args.at(key).size()) {
				val = ValueGetter<T>::get(args.at(key), N);
				return true;
			} else {
				return false;
			}
		}

		std::map<std::string, std::vector<std::string>> args;
		bool init = false;

		static CommandLineArgs global;	
	};

	SIBR_SYSTEM_EXPORT const CommandLineArgs & getCommandLineArgs();
	

	/// Template Arg class, will init itself in the defaut ctor using the command line args (ie. --key value)
	/// should be declared as some class member using Arg<T> myArg = { "key", some_default_value };
	/// is implicitly convertible to the template type
	/// \note Note : as multiple implicit conversion is not possible in cpp, you might have to use the .get() method to access the inner T value
	template<typename T>
	class Arg {
	public:
		Arg(const std::string & key, const T & default_value) {
			value = getCommandLineArgs().get<T>(key, default_value);
		}

		operator const T &() const { return value; }
		const T & get() const { return value; }
		T & operator=(const T & t) { value = t; return value; }

	protected:
		T value;
	};

	/// helper for RequiredArg class
	template<typename T>
	class RequiredArgBase {
	public:
		RequiredArgBase(const std::string & _key) : key(_key) {
			if (getCommandLineArgs().contains(key)) {
				value = getCommandLineArgs().get<T>(key, value);
				wasInit = true;
			}
		}

		operator const T &() const { checkInit(); return value; }
		const T & get() const { checkInit(); return value; }
		T & operator=(const T & t) { value = t; wasInit = true; return value; }

		const bool & isInit() const { return wasInit; }
	protected:
		void checkInit() const {
			if (!wasInit) {
				SIBR_ERR << "arg <" << key << "> is required " << std::endl;
			}
		}

		std::string key;
		T value;
		bool wasInit = false;
	};

	/// same as Arg expect this one will crash if attempt to use the value while not initialized
	/// initialization can be done using the command line or manually
	template<typename T>
	class RequiredArg : public RequiredArgBase<T> {
		using RequiredArgBase<T>::RequiredArgBase;
	};

	///specialization required for std::string as const string & key constructor and const T & constructor are ambiguous.
	template<>
	class RequiredArg<std::string> : public RequiredArgBase<std::string> {
		RequiredArg(const RequiredArg &) = delete;
		RequiredArg & operator=(const RequiredArg &) = delete;
		
		using RequiredArgBase<std::string>::RequiredArgBase;
		
	public:
		RequiredArg & operator=(const std::string & t) { value = t; wasInit = true;  return *this; }
		operator const char*() const { checkInit(); return value.c_str(); }
	};

	/// Hierarchy of Args classes that can be seens as modules, and can be combined using virtual inheritance, with no duplication of code so derived Args has no extra work to do
	///assuming CommandLineArgs::parseMainArgs() was called once, Args arguments will be automatically initialized with the value from the command line by the constructor
	///
	///existing Args structs should cover most of the existing IBR apps
	///
	///to add a new argument like --my-arg 5 on top of existing arguments 
	///and
	///to add a new required argument like --important-param "on" on top of existing arguments
	///
	/// struct SIBR_SYSTEM_EXPORT MyArgs : virtual ExistingArg1, virtual ExistingArgs2, ... {
	///		Arg<int> myParameter = { "my-arg", some_default_value };
	///		RequiredArg<std::string> myRequiredParameter = { "important-param" };
	/// }

	struct SIBR_SYSTEM_EXPORT AppArgs {
		AppArgs();

		std::string appName;
		std::string appPath;
	};

	struct SIBR_SYSTEM_EXPORT WindowArgs {
		Arg<int> win_width = { "width", 720 };
		Arg<int> win_height = { "height", 480 };
		Arg<int> vsync = { "vsync", 0 };
		Arg<bool> fullscreen = { "fullscreen", false };
		Arg<bool> hdpi = { "hd", false };
	};

	struct WindowAppArgs : 
		virtual AppArgs, virtual WindowArgs {
	};

	struct SIBR_SYSTEM_EXPORT RenderingArgs {
		Arg<std::string> scene_metadata_filename = { "scene", "scene_metadata.txt" };
		Arg<std::array<int, 2>> rendering_size = { "rendering-size", { 720, 480 } };
		Arg<int> texture_width = { "texture-width", 0 };
		Arg<float> texture_ratio = { "texture-ratio", 1.0f };
		Arg<int> rendering_mode = { "rendering-mode", RENDERMODE_MONO };
		Arg<sibr::Vector3f> focal_pt = { "focal-pt", {0.0f, 0.0f, 0.0f} };
	};

	struct SIBR_SYSTEM_EXPORT BasicDatasetArgs {
		RequiredArg<std::string> dataset_path = { "path" };
	};

	struct BasicIBRAppArgs : 
		virtual WindowAppArgs, virtual BasicDatasetArgs, virtual RenderingArgs {
	};

	//specializations of ValueGetter<T>

	template<>
	struct ValueGetter<std::string> {
		static std::string get(const std::vector<std::string> & values, uint n) {
			return values[n];
		}
	};

	template<>
	struct ValueGetter<bool> {
		static bool get(const std::vector<std::string> & values, uint n) {
			return true;
		}
	};

	template<>
	struct ValueGetter<double> {
		static double get(const std::vector<std::string> & values, uint n) {
			return std::stod(values[n]);
		}
	};

	template<>
	struct ValueGetter<float> {
		static float get(const std::vector<std::string> & values, uint n) {
			return std::stof(values[n]);
		}
	};

	template<>
	struct ValueGetter<int> {
		static int get(const std::vector<std::string> & values, uint n) {
			return std::stoi(values[n]);
		}
	};

	template<>
	struct ValueGetter<char> {
		static char get(const std::vector<std::string> & values, uint n) {
			return static_cast<char>(std::stoi(values[n]));
		}
	};

	template<>
	struct ValueGetter<uint> {
		static uint get(const std::vector<std::string> & values, uint n) {
			return static_cast<uint>(std::stoi(values[n]));
		}
	};

	template<typename T, uint N>
	struct ValueGetter<std::array<T, N>> {
		static std::array<T, N> get(const std::vector<std::string> & values, uint n) {
			std::array<T, N> out;
			for (uint i = 0; i < N; ++i) {
				out[i] = ValueGetter<T>::get(values, n*N*NumberOfArg<T> + i);
			}
			return out;
		}
	};


	template<typename T, uint N>
	struct ValueGetter<sibr::Vector<T, N>> {
		static sibr::Vector<T, N> get(const std::vector<std::string> & values, uint n) {
			sibr::Vector<T, N> out;
			for (uint i = 0; i < N; ++i) {
				out[i] = ValueGetter<T>::get(values, n*N*NumberOfArg<T> + i);
			}
			return out;
		}
	};

} // namespace sibr
