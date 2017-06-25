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
/**
 * Plot std::vector's using gnuplot
 */
class CPlotter
{
public:
	CPlotter() noexcept;
	~CPlotter() noexcept;

	void operator ()(const std::string& unaliased command) noexcept;
	void SendCommand(const std::string& unaliased command) noexcept;

	template<typename T>
	using vec = std::vector<T>;

	template<typename T>
	using mat = vec<vec<T>>;

	template<typename T>
	using vmat = vec<vec<vec<T>>>;

	template <typename T, typename U>
	void Plot(vec<T>& unaliased y, vec<U>& unaliased x,
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
	void Plot(mat<T>& unaliased y, vec<U>& unaliased x,
			const std::string& unaliased title = "",
			const std::string& unaliased xlabel = "x",
			const std::string& unaliased ylabel = "y",
			const vec<std::string>& unaliased legend = vec<std::string>()) noexcept
	{
		std::string aux_filename = "_aux";
		WritePlotFile(aux_filename, y, x);
		aux_filename += ".txt";

		std::string command = "";
		for (size_t j = 0; j < y.size(); ++j)
		{
			std::string plot_string = (j == 0 ? "plot " : "");
			std::string plot_command = plot_string + "'" + aux_filename + "' using 1:" + std::to_string(j + 2);

			command += plot_command;
			if (legend.size())
				command += "title '" + legend[j] + "'";
			command +=  " with line lt 1 lc " + std::to_string(j + 1) + ", ";
		}

		SendCommand(command);
		SendCommand("set title '" + title + "'");
		SendCommand("set xlabel '" + xlabel + "'");
		SendCommand("set ylabel '" + ylabel + "'");
	}

	template <typename T, typename U>
	void Plot(const size_t layout, vmat<T>& unaliased y, vec<U>& unaliased x,
			const std::string& unaliased title = "",
			const std::string& unaliased xlabel = "x",
			const std::string& unaliased ylabel = "y",
			const mat<std::string>& unaliased legend = mat<std::string>()) noexcept
	{
		std::string aux_filename = "_aux";
		WritePlotFile(aux_filename, y, x);
		aux_filename += ".txt";
		SendCommand("set multiplot layout " + std::to_string(layout / 10) + ", " + std::to_string(layout % 10));

		for (size_t k = 0; k < y.size(); ++k)
		{
			std::string command = "";
			for (size_t j = 0; j < y[0].size(); ++j)
			{
				std::string plot_string = (j == 0 ? "plot " : "");
				std::string plot_command = plot_string + "'" + aux_filename + "' using 1:" + std::to_string(k + j * y.size() + 2);

				command += plot_command;
				if (legend.size())
					command += "title '" + legend[k][j] + "'";
				command +=  " with line lt 1 lc " + std::to_string(j + 1) + ", ";
			}

			SendCommand(command);
			SendCommand("set title '" + title + "'");
			SendCommand("set xlabel '" + xlabel + "'");
			SendCommand("set ylabel '" + ylabel + "'");
		}
	}
private:
	FILE *gnuplotpipe;

	template <typename T, typename U>
	static void WritePlotFile(const std::string& unaliased file_name, vec<T>& unaliased y, vec<U>& unaliased x) noexcept
	{
		const string filename_x = file_name + ".txt";
		ofstream myfile(filename_x.c_str());
		if (!myfile.is_open())
		{
			std::cerr << "Unable to open file";
			return;
		}

		for (size_t i = 0; i < y.size(); ++i)
			myfile << x[i] << "\t" << y[i] << std::endl;

		myfile.close();
	}

	template <typename T, typename U>
	static void WritePlotFile(const std::string& unaliased file_name, mat<T>& unaliased y, vec<U>& unaliased x) noexcept
	{
		const string filename_x = file_name + ".txt";
		ofstream myfile(filename_x.c_str());
		if (!myfile.is_open())
		{
			std::cerr << "Unable to open file";
			return;
		}

		for (size_t i = 0; i < y[0].size(); ++i)
		{
			myfile << x[i];
			for (size_t j = 0; j < y.size(); ++j)
				 myfile << "\t" << y[j][i];
			myfile << std::endl;
		}

		myfile.close();
	}

	template <typename T, typename U>
	static void WritePlotFile(const std::string& unaliased file_name, vmat<T>& unaliased y, vec<U>& unaliased x) noexcept
	{
		const string filename_x = file_name + ".txt";
		ofstream myfile(filename_x.c_str());
		if (!myfile.is_open())
		{
			std::cerr << "Unable to open file";
			return;
		}

		for (size_t i = 0; i < y[0][0].size(); ++i)
		{
			myfile << x[i];
			for (size_t j = 0; j < y[0].size(); ++j)
				for (size_t k = 0; k < y.size(); ++k)
					myfile << "\t" << y[k][j][i];
			myfile << std::endl;
		}

		myfile.close();
	}

	static void DeletePlotFile(const std::string& unaliased file_name) noexcept;
};


#endif /* UTILITIES_CPLOTTER_H_ */
