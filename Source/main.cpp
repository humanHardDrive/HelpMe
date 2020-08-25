#include <iostream>

#include <wx/msgdlg.h>

int main(int argc, char** argv)
{
	wxMessageDialog dlg(nullptr, "HELLO", wxMessageBoxCaptionStr, wxOK);
	dlg.ShowModal();

	return 0;
}