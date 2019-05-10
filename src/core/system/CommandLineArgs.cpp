
#include "core/system/CommandLineArgs.hpp"
#include "core/system/Utils.hpp"

namespace sibr
{
	CommandLineArgs CommandLineArgs::global = CommandLineArgs();

	const CommandLineArgs & getCommandLineArgs()
	{
		return CommandLineArgs::getGlobal();
	}

	const CommandLineArgs & CommandLineArgs::getGlobal()
	{
		static bool first = true;
		if (!global.init && first) {
			SIBR_WRG << "CommandLineArgs::parseMainArgs(ac, av) was not called rigth after main(ac, av) \n default value (empty command line) will be used" << std::endl;
			first = false;
		}
		return global;
	}

	void CommandLineArgs::parseMainArgs(const int argc, const char * const * argv)
	{
		static const std::vector<std::string> acceptable_prexifes = { "--", "-" };

		global.args.clear();

		global.args["app_path"] = { std::string(argv[0])};

		std::string current_arg = "";
		for (int i = 1; i < argc; ++i) {
			std::string arg = std::string(argv[i]);
			bool new_arg = false;
			for (const auto & prefix : acceptable_prexifes) {
				if (arg.substr(0, prefix.size()) == prefix) {
					current_arg = arg.substr(prefix.size());
					new_arg = true;		
					break;
				}
			}
			if (current_arg == "") {
				continue;
			}
			if (new_arg) {
				if (global.args.count(current_arg) > 0) {
					SIBR_WRG << "collision for argument : " << arg << std::endl;
				} else {
					global.args[current_arg] = {};
				}				
			} else {
				global.args[current_arg].push_back(arg);
			}
		}

		global.init = true;
	}

	bool CommandLineArgs::contains(const std::string & key) const
	{
		return args.count(key) > 0;
	}

	int CommandLineArgs::numArguments(const std::string & key) const
	{
		if (contains(key)) {
			return (int)args.at(key).size();
		} else {
			return -1;
		}
	}

	AppArgs::AppArgs()
	{
		Path path = getCommandLineArgs().getRequired<std::string>("app_path");
		appName = path.filename().string();
		appPath = path.parent_path().string();
	}

} // namespace sirb

