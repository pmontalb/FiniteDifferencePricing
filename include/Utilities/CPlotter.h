/*
 * CPlotter.h
 *
 *  Created on: 16 Jun 2017
 *      Author: raiden
 */

#ifndef UTILITIES_CPLOTTER_H_
#define UTILITIES_CPLOTTER_H_

#include <Flags.h>


#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
class CPlotter
{
public:
	CPlotter() noexcept;
	~CPlotter() noexcept;

	void operator ()(const std::string& unaliased command);
	void SendCommand(const std::string& unaliased command) noexcept;

	template <typename T, typename U>
	void plot(std::vector<T>& unaliased y, std::vector<U>& unaliased x,
			const std::string& unaliased title = "",
			const std::string& unaliased xlabel = "x",
			const std::string& unaliased ylabel = "y",
			const std::string& unaliased legend = "") noexcept
	{
		const string aux_filename = "_aux";
		WritePlotFile(aux_filename, y, x);

		SendCommand("set title '" + title + "'");
		SendCommand("set xlabel '" + xlabel + "'");
		SendCommand("set ylabel '" + ylabel + "'");
		SendCommand("plot '" + aux_filename + ".txt" + "' title '" + legend + "' with line lt 1 lc 7");
	}
private:
	FILE *gnuplotpipe;
private:
	template <typename T, typename U>
	static void WritePlotFile(const std::string& unaliased file_name, std::vector<T>& unaliased y, std::vector<U>& unaliased x) noexcept
	{
		const string filename_x = file_name + ".txt";
		ofstream myfile(filename_x.c_str());
		if (myfile.is_open())
		{
			for (size_t i = 0; i < y.size(); ++i)
				myfile << x[i] << "\t" << y[i] << std::endl;

			myfile.close();
		}
		else
		{
			std::cerr << "Unable to open file";
			return;
		}
	}

	template <typename T, typename U>
	static void WritePlotFile(const std::string& unaliased file_name, std::vector<std::vector<T>>& unaliased y, std::vector<U>& unaliased x) noexcept
	{
		const string filename_x = file_name + ".txt";
		ofstream myfile(filename_x.c_str());
		if (myfile.is_open())
		{
			for (size_t i = 0; i < y.size(); ++i)
			{
				myfile << x[i];
				for (size_t j = 0; j < y.size(); ++j)
					 myfile << "\t" << y[j][i] << std::endl;
			}

			myfile.close();
		}
		else
		{
			std::cerr << "Unable to open file";
			return;
		}
	}

	static void DeletePlotFile(const std::string& unaliased file_name) noexcept;
};


#endif /* UTILITIES_CPLOTTER_H_ */
