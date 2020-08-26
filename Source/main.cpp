#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>

#include <wx/msgdlg.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>

struct ScriptInfo
{
	std::string sName;
	std::string sDescription;
	std::vector<std::string> aTags;
};

std::map<std::string, std::function<std::string(const std::string&)>> dialogGenMap =
{
	{
		"OK",
		[](const std::string& sText)
		{
			wxMessageDialog(nullptr, sText, "").ShowModal();
			return "";
		}
	},
	{
		"YesNo", 
		[](const std::string& sQuestion)
		{
			int nRes = wxMessageDialog(nullptr, sQuestion, "", wxYES_NO).ShowModal();

			if (nRes == wxID_YES)
				return "Yes";
			else if (nRes == wxID_NO)
				return "No";
			else
				return "";
		}
	}
};

int main(int argc, char** argv)
{
	boost::property_tree::ptree pt;
	boost::property_tree::ptree flowRoot;

	std::vector<ScriptInfo> scriptList;
	std::map<std::string, std::vector<ScriptInfo*>> tagMap;

	//List out the product options
	if (argc > 1)
	{
		std::cout << "Opening directory " << argv[1] << std::endl;
		std::cout << "Products found:" << std::endl;
		for (auto it = std::filesystem::directory_iterator(argv[1]); it != std::filesystem::directory_iterator(); it++)
		{
			std::filesystem::path p = *it;
			if (it->is_directory() && p.extension().string() == ".product")
				std::cout << p.filename().string() << std::endl;
		}

		std::cout << std::endl;
	}

	//List out the scripts for a specific product
	if (argc > 2)
	{
		//Open the product specific directory
		std::filesystem::path productsPath(argv[1]);
		productsPath /= argv[2];

		std::cout << "Opening product " << argv[2] << std::endl;
		std::cout << "Scripts found:" << std::endl;
		for (auto it = std::filesystem::directory_iterator(productsPath); it != std::filesystem::directory_iterator(); it++)
		{
			std::filesystem::path p = *it;
			//It needs to be a .script folder
			if (it->is_directory() && p.extension().string() == ".script")
			{
				boost::property_tree::ptree pt;

				//Build up the script info list
				try
				{
					ScriptInfo scriptInfo;
					typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
					boost::char_separator<char> token(",");

					//Grab the actual script info
					boost::property_tree::read_xml(p.string() + "\\script.xml", pt);
					std::cout << p.filename().string() << std::endl;

					scriptInfo.sName = p.filename().string();
					scriptInfo.sDescription = pt.get<std::string>("Description", "");
					//Tags are comma separated
					std::string sTags = pt.get<std::string>("Tags", "");

					//Tokenize tag list
					tokenizer tokens(sTags, token);
					for (tokenizer::iterator it = tokens.begin(); it != tokens.end(); it++)
						scriptInfo.aTags.push_back(*it);

					//Add script info to list
					scriptList.push_back(scriptInfo);
				}
				catch (boost::property_tree::ptree_error& e)
				{
					std::cout << "Could not parse " << p.filename().string() << std::endl;
				}
			}
		}

		std::cout << std::endl;
	}

	if (pt.get_child_optional("flow"))
	{
		bool bPathDefined = true;
		flowRoot = pt.get_child("flow");

		while (bPathDefined)
		{
			std::string sType = flowRoot.get<std::string>("Type", "");
			std::string sRes = "";

			bPathDefined = false;
			if (dialogGenMap.find(sType) != dialogGenMap.end())
				sRes = dialogGenMap[sType](flowRoot.get<std::string>("Text", ""));
			else
				dialogGenMap["OK"]("No dialog defined");

			if (!sRes.empty() && flowRoot.get_child_optional(sRes))
			{
				bPathDefined = true;
				flowRoot = flowRoot.get_child(sRes);
			}

			if (!bPathDefined)
				dialogGenMap["OK"]("No path defined");
		}

	}
	else
	{
		dialogGenMap["OK"]("No flow defined");
	}

	return 0;
}