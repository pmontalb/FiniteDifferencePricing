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

	void operator ()(const std::string& unaliased command) noexcept;
	void SendCommand(const std::string& unaliased command) noexcept;

	template <typename T, typename U>
	void Plot(std::vector<T>& unaliased y, std::vector<U>& unaliased x,
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

	template <typename T, typename U>
	void Plot(std::vector<std::vector<T>>& unaliased y, std::vector<U>& unaliased x,
			const std::string& unaliased title = "",
			const std::string& unaliased xlabel = "x",
			const std::string& unaliased ylabel = "y",
			const std::vector<std::string>& unaliased legend = std::vector<std::string>()) noexcept
	{
		std::string aux_filename = "_aux";
		WritePlotFile(aux_filename, y, x);
		aux_filename += ".txt";

		std::string command = "";
		for (size_t j = 0; j < y.size(); ++j)
		{
			std::string plot_string = (j == 0 ? "plot" : "");
			std::string plot_command = plot_string + "'" + aux_filename + "' using 1:" + std::to_string(j + 2);

			command += plot_command + " notitle with line lt 1 lc " + std::to_string(j + 1) + ", ";
			if (legend.size())
				command += "' title '" + legend[j] + "'";
		}

		SendCommand(command);
		SendCommand("set title '" + title + "'");
		SendCommand("set xlabel '" + xlabel + "'");
		SendCommand("set ylabel '" + ylabel + "'");
	}
private:
	FILE *gnuplotpipe;

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
			for (size_t i = 0; i < y[0].size(); ++i)
			{
				myfile << x[i];
				for (size_t j = 0; j < y.size(); ++j)
					 myfile << "\t" << y[j][i];
				myfile << std::endl;
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
