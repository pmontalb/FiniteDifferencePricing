/*
 * CPlotter.cpp
 *
 *  Created on: 16 Jun 2017
 *      Author: raiden
 */


#include <Utilities/CPlotter.h>

CPlotter::CPlotter() noexcept
{
	gnuplotpipe = popen("gnuplot -persistent", "w");
	if (!gnuplotpipe)
		std::cerr << ("Gnuplot not found !");
}

CPlotter::~CPlotter() noexcept
{
	fprintf(gnuplotpipe, "exit\n");
	pclose(gnuplotpipe);

	DeletePlotFile("_aux");
}

void CPlotter::DeletePlotFile(const std::string& unaliased file_name) noexcept
{
	string file_x = file_name + ".txt";
	remove(file_x.c_str());
}

void CPlotter::SendCommand(const string & unaliased command) noexcept
{
	fprintf(gnuplotpipe, "%s\n", command.c_str());
	fflush(gnuplotpipe);
}
