#include "api/iode.h"
#include "api/iodeapi.h"

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>


int main(int argc, char **argv)
{
	int size_char = sizeof(char);
	int size_char_ptr = sizeof(char*);
	int size_short = sizeof(short);
	int size_long = sizeof(long);
	int size_real = sizeof(double);

	std::ofstream output;
	std::string filename = "iode_structs_";
#ifdef ALIGN1
	filename += "alignment_1_byte" ;
#else
	filename += "default_alignment";
#endif
	filename += ".txt";
  	output.open(filename);

	if(output.is_open())
	{
		output << "SWHDL   o_val     " << sizeof(SWHDL) << std::endl;
		output << "ONAME   o_name    " << sizeof(ONAME) << std::endl;
		output << "char    o_pad[3]  " << sizeof(char[3]) << std::endl;
		output << ""                   << std::endl;
		output << "SUM =             " << sizeof(SWHDL) + sizeof(ONAME) + sizeof(char[3]) << std::endl;
		output << "size of KOBJ:     " << sizeof(KOBJ) << std::endl;
		output << "alignof KOBJ:     " << alignof(KOBJ) << std::endl;
		output << "------------------------------" << std::endl;
		output << "KOBJ*  k_objs             " << sizeof(KOBJ*) << std::endl;
		output << "long   k_nb               " << sizeof(long) << std::endl;
		output << "short  k_type             " << sizeof(short) << std::endl;
		output << "short  k_mode             " << sizeof(short) << std::endl;
		output << "char   k_arch[LMAGIC]     " << sizeof(char[LMAGIC]) << std::endl;
		output << "char   k_magic[LMAGIC]    " << sizeof(char[LMAGIC]) << std::endl;
		output << "OFNAME k_oname            " << sizeof(OFNAME) << std::endl;
		output << "char   k_desc[K_MAX_DESC] " << sizeof(char[K_MAX_DESC]) << std::endl;
		output << "char   k_data[K_MAX_DESC] " << sizeof(char[K_MAX_DESC]) << std::endl;
		output << "char   k_compressed       " << sizeof(char) << std::endl;
		output << "char   k_reserved[59]     " << sizeof(char[59]) << std::endl;
		output << "char*  k_nameptr          " << sizeof(char*) << std::endl;
		output << ""                           << std::endl;
		output << "SUM =                     " << sizeof(KOBJ*) + sizeof(long) + (2 * sizeof(short)) +
													(2 * sizeof(char[LMAGIC])) + sizeof(OFNAME) +
													(2 * sizeof(char[K_MAX_DESC])) + sizeof(char) +
													sizeof(char[59]) + sizeof(char*) << std::endl;
		output << "size of KDB:              " << sizeof(KDB) << std::endl;
		output << "alignof KDB:              " << alignof(KDB) << std::endl;
		output << ""                           << std::endl;
		output << "------------------------------" << std::endl;
		output << "long    p_y       " << sizeof(long) << std::endl;
		output << "long    p_s       " << sizeof(long) << std::endl;
		output << "char    p_p       " << sizeof(char) << std::endl;
		output << "char    p_pad[3]  " << sizeof(char[3]) << std::endl;
		output << ""                   << std::endl;
		output << "SUM =             " << (2 * sizeof(long)) + sizeof(char) + sizeof(char[3]) << std::endl;
		output << "size of PERIOD:   " << sizeof(PERIOD) << std::endl;
		output << "alignof PERIOD:   " << alignof(PERIOD) << std::endl;
		output << "------------------------------" << std::endl;
		output << "PERIOD  s_p1      " << sizeof(PERIOD) << std::endl;
		output << "PERIOD  s_p2      " << sizeof(PERIOD) << std::endl;
		output << "short   s_nb      " << sizeof(short) << std::endl;
		output << "char    s_pad[2]  " << sizeof(char[2]) << std::endl;
		output << ""                   << std::endl;
		output << "SUM =             " << (2 * sizeof(PERIOD)) + sizeof(short) + sizeof(char[2]) << std::endl;
		output << "size of SAMPLE:   " << sizeof(SAMPLE) << std::endl;
		output << "alignof SAMPLE:   " << alignof(SAMPLE) << std::endl;
		output << ""                   << std::endl;
		output << "------------------------------" << std::endl;
		output << "typedef char* CMT " << sizeof(char*) << std::endl;
		output << "size of CMT:      " << sizeof(CMT) << std::endl;
		output << "alignof CMT:      " << alignof(CMT) << std::endl;
		output << "------------------------------" << std::endl;
		output << "long    tot_lg    " << sizeof(long) << std::endl;
		output << "long    exec_lg   " << sizeof(long) << std::endl;
		output << "short   nb_names  " << sizeof(short) << std::endl;
		output << "char    dupendo   " << sizeof(char) << std::endl;
		output << "char    pad       " << sizeof(char) << std::endl;
		output << "LNAME   lnames[1] " << sizeof(LNAME[1]) << std::endl;
		output << ""                   << std::endl;
		output << "SUM             = " << (2 * size_long) + size_short + (2 * size_char) + 
											sizeof(LNAME[1]) << std::endl;
		output << "size of CLEC:     " << sizeof(CLEC) << std::endl;
		output << "alignof CLEC:     " << alignof(CLEC) << std::endl;
		output << "------------------------------" << std::endl;
		output << "char* lec         " << sizeof(char*) << std::endl;
		output << "CLEC* clec        " << sizeof(CLEC*) << std::endl;
		output << ""                   << std::endl;
		output << "size of IDT:      " << sizeof(IDT) << std::endl;
		output << "alignof IDT:      " << alignof(IDT) << std::endl;
		output << "------------------------------" << std::endl;
		output << "char*   lec       " << sizeof(char*) << std::endl;
		output << "CLEC*   clec      " << sizeof(CLEC*) << std::endl;
		output << "char    solved    " << sizeof(char) << std::endl;
		output << "char    method    " << sizeof(char) << std::endl;
		output << "SAMPLE  smpl      " << sizeof(SAMPLE) << std::endl;
		output << "char*   cmt       " << sizeof(char*) << std::endl;
		output << "char*   blk       " << sizeof(char*) << std::endl;
		output << "char*   instr     " << sizeof(char*) << std::endl;
		output << "long    date      " << sizeof(long) << std::endl;
		output << "float   tests[EQS_NBTESTS]" << sizeof(float[EQS_NBTESTS]) << std::endl;
		output << ""                   << std::endl;
		output << "SUM =             " << size_char_ptr + sizeof(CLEC*) + (2 * size_char) + sizeof(SAMPLE) + 
											3 * size_char_ptr + size_long + sizeof(float[EQS_NBTESTS]) << std::endl;
		output << "size of EQ:       " << sizeof(EQ) << std::endl;
		output << "alignof EQ:       " << alignof(EQ) << std::endl;
		output << "------------------------------" << std::endl;
		output << "typedef char* LIS " << sizeof(char*) << std::endl;
		output << "size of LIS:      " << sizeof(LIS) << std::endl;
		output << "alignof LIS:      " << alignof(LIS) << std::endl;
		output << "------------------------------" << std::endl;
		output << "double val     " << sizeof(double) << std::endl;
		output << "double relax   " << sizeof(double) << std::endl;
		output << "double std     " << sizeof(double) << std::endl;
		output << ""                   << std::endl;
		output << "SUM =             " << 3 * size_real << std::endl;
		output << "size of SCL:      " << sizeof(SCL) << std::endl;
		output << "alignof SCL:      " << alignof(SCL) << std::endl;
		output << "------------------------------" << std::endl;
		output << "char*  tc_val     " << sizeof(char*) << std::endl;
		output << "char   tc_type    " << sizeof(char) << std::endl;
		output << "char   tc_attr    " << sizeof(char) << std::endl;
		output << "char   tc_pad[2]  " << sizeof(char[2]) << std::endl;
		output << ""                   << std::endl;
		output << "SUM =             " << size_char_ptr + (2 * size_char) + sizeof(char[2]) << std::endl;
		output << "size of TCELL:    " << sizeof(TCELL) << std::endl;
		output << "alignof TCELL:    " << alignof(TCELL) << std::endl;
		output << "------------------------------" << std::endl;
		struct S
		{
			U_ch axis:1;
			U_ch pbyte:7;
		};
		output << "char*  tl_val     " << sizeof(char*) << std::endl;
		output << "char   tl_type    " << sizeof(char) << std::endl;
		output << "char   tl_graph   " << sizeof(char) << std::endl;
		output << "U_ch   tl_axis:1  " << "" << std::endl;
		output << "U_ch   tl_pbyte:7 " << sizeof(S) << std::endl;
		output << "char   tl_pad[1]  " << sizeof(char[1]) << std::endl;
		output << ""                   << std::endl;
		output << "SUM =             " << size_char_ptr + (2 * size_char) + sizeof(S) + sizeof(char[1]) << std::endl;
		output << "size of TLINE:    " << sizeof(TLINE) << std::endl;
		output << "alignof TLINE:    " << alignof(TLINE) << std::endl;
		output << "------------------------------" << std::endl;
		output << "short   t_lang    " << sizeof(short) << std::endl; 
		output << "short   t_free    " << sizeof(short) << std::endl; 
		output << "short   t_nc      " << sizeof(short) << std::endl; 
		output << "short   t_nl      " << sizeof(short) << std::endl; 
		output << "TLINE   t_div     " << sizeof(TLINE) << std::endl; 
		output << "TLINE*  t_line    " << sizeof(TLINE*) << std::endl; 
		output << "float   t_zmin    " << sizeof(float) << std::endl; 
		output << "float   t_zmax    " << sizeof(float) << std::endl; 
		output << "float   t_ymin    " << sizeof(float) << std::endl; 
		output << "float   t_ymax    " << sizeof(float) << std::endl; 
		output << "char    t_attr    " << sizeof(char) << std::endl; 
		output << "char    t_box     " << sizeof(char) << std::endl; 
		output << "char    t_shadow  " << sizeof(char) << std::endl; 
		output << "char    t_gridx   " << sizeof(char) << std::endl; 
		output << "char    t_gridy   " << sizeof(char) << std::endl; 
		output << "char    t_axis    " << sizeof(char) << std::endl; 
		output << "char    t_align   " << sizeof(char) << std::endl; 
		output << "char    t_pad[13] " << sizeof(char[13]) << std::endl; 
		output << ""                   << std::endl;
		output << "SUM =             " << (4 * size_short) + sizeof(TLINE) + sizeof(TLINE*) + (4 * size_long) + 
											(7 * size_char) + sizeof(char[13]) << std::endl;
		output << "size of TBL:      " << sizeof(TBL) << std::endl;
		output << "alignof TBL:      " << alignof(TBL) << std::endl;
		output << "------------------------------" << std::endl;
		output << "typedef double* VAR " << sizeof(double*) << std::endl;
		output << "size of VAR:           " << sizeof(VAR) << std::endl;
		output << "alignof VAR:           " << alignof(VAR) << std::endl;
		output << ""                        << std::endl;
		output << "------------------------------" << std::endl;
		std::string st = "hello";
		output << "size of std::string 'hello'   " << sizeof(st) << std::endl;
		output << "alignof std::string 'hello'   " << alignof(std::string) << std::endl;

		output.close();
	}
	else
		std::cout << "Unable to open the file " + filename << std::endl;

	// print content in console
	std::string line;
	std::ifstream output_(filename);

	if(output_.is_open())
	{
		while(getline(output_,line))
			std::cout << line << std::endl;
		output_.close();
	}
	else 
		std::cout << "Unable to open file " + filename << std::endl; 
}