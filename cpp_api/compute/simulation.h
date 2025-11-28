#pragma once

#include "cpp_api/common.h"
#include "api/utils/utils.h"
#include "api/time/sample.h"
#include "cpp_api/KDB/kdb_lists.h"


class Simulation: public CSimulation
{
public:
    Simulation() : CSimulation() 
    {
        if (KSIM_EXO) 
        {
            SCR_free_tbl((unsigned char**) KSIM_EXO);
            KSIM_EXO = NULL;
        }        
    }

    ~Simulation() 
    {
        if (KSIM_EXO) 
        {
            SCR_free_tbl((unsigned char**) KSIM_EXO);
            KSIM_EXO = NULL;
        }
    }

    // ==== MODEL SIMULATION ====

    bool model_exchange(const std::string& list_exo = "");

    bool model_compile(const std::string& list_eqs = "");

    bool model_simulate(const std::string& from, const std::string& to, const std::string& list_eqs = "");

    bool model_calculate_SCC(const int nb_iterations, const std::string& pre_name = "_PRE", const std::string& inter_name = "_INTER", 
        const std::string& post_name = "_POST", const std::string& list_eqs = "");

    bool model_simulate_SCC(const std::string& from, const std::string& to, const std::string& pre_name = "_PRE", 
        const std::string& inter_name = "_INTER", const std::string& post_name = "_POST");
};
