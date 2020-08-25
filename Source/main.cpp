#include <iostream>
#include <map>
#include <string>

#include <wx/msgdlg.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

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

	if (argc < 2)
	{
		std::cout << "No file specified" << std::endl;
		return 1;
	}

	try
	{
		boost::property_tree::read_xml(argv[1], pt);
	}
	catch (boost::property_tree::ptree_error& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	std::cout << "Description: " << pt.get<std::string>("Description", "") << std::endl;
	std::cout << "Tags: " << pt.get<std::string>("Tags", "") << std::endl;

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