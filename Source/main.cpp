#include <iostream>
#include <map>
#include <string>

#include <wx/msgdlg.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

std::map<std::string, std::function<int(const std::string&)>> dialogGenMap =
{
	{
		"OK",
		[](const std::string& sText)
		{
			return wxMessageDialog(nullptr, sText, "").ShowModal();
		}
	},
	{
		"YesNo", 
		[](const std::string& sQuestion)
		{
			return wxMessageDialog(nullptr, sQuestion, "", wxYES_NO).ShowModal();
		}
	}
};

int main(int argc, char** argv)
{
	boost::property_tree::ptree pt;

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
	}

	return 0;
}