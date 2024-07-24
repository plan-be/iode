#include "pch.h"


class SimulationTest : public KDBTest, public ::testing::Test
{
protected:
    Simulation sim;
    std::string from;
    std::string to;
    std::string endo_exo;

    void SetUp() override
    {
        KDBEquations kdb_eqs(input_test_dir + "fun.ae");
        KDBScalars kdb_scl(input_test_dir + "fun.as");
        KDBVariables kdb_var(input_test_dir + "fun.av");

        from = "2000Y1";
        to = "2002Y1";

        endo_exo = "UY-XNATY";

        // Simulation std parameters
        KSIM_START = VAR_INIT_TM1;
        KSIM_EPS = 0.0001;
        KSIM_MAXIT = 100;
        KSIM_RELAX = 0.7;
        KSIM_SORT = SORT_BOTH;
        KSIM_PASSES = 5;
        KSIM_DEBUG = 1;
    }

    // void TearDown() override {}
};

TEST_F(SimulationTest, ModelExchange)
{
    sim.model_exchange(endo_exo);
    EXPECT_EQ(SCR_tbl_size((unsigned char**) KSIM_EXO), 1);
    EXPECT_EQ(std::string(KSIM_EXO[0]), "UY-XNATY");

    // reset KSIM_EXO
    sim.model_exchange();
    EXPECT_TRUE(KSIM_EXO == NULL);
}

TEST_F(SimulationTest, Simulation)
{
    sim.set_sort(SimuSortAlgorithm::SORT_NONE);
    sim.set_sort(SimuSortAlgorithm::SORT_CONNEX);
    sim.set_sort(SimuSortAlgorithm::SORT_BOTH);

    sim.set_initialization_method(VariablesInitialization::VAR_INIT_TM1);
    sim.set_initialization_method(VariablesInitialization::VAR_INIT_TM1_A);
    sim.set_initialization_method(VariablesInitialization::VAR_INIT_TM1_NA);
    sim.set_initialization_method(VariablesInitialization::VAR_INIT_ASIS);
    sim.set_initialization_method(VariablesInitialization::VAR_INIT_EXTRA);
    sim.set_initialization_method(VariablesInitialization::VAR_INIT_EXTRA_A);
    sim.set_initialization_method(VariablesInitialization::VAR_INIT_EXTRA_NA);

    // Invalid arguments
    // invalid sample definition
    EXPECT_THROW(sim.model_simulate("2000U1", to), std::invalid_argument);
    // invalid list of equations
    EXPECT_THROW(sim.model_simulate(from, to, "$UNKNOWN_LIST"), IodeExceptionInvalidArguments);

    // Test simulation: divergence
    KSIM_MAXIT = 2;
    EXPECT_THROW(sim.model_simulate(from, to), IodeExceptionFunction);

    // Check _PRE list after simulation (prolog)
    std::string lst_pre = Lists.get("_PRE");
    std::string expected_lst_pre = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    EXPECT_EQ(lst_pre, expected_lst_pre);

    // Check _DIVER list (divergent equations)
    std::string lst_diver = Lists.get("_DIVER");
    std::string expected_lst_diver = "SSH3O,WBG,SSF3,YDH,DTH,YDTG,YSFIC,WMIN,WLCP,WBGP,YSEFT2,YSEFT1,YSEFP,SBG,PWBG,W,ZJ,QMT,QI5,QC_,SSFG,YDH_,SG,ACAG,FLG";
    EXPECT_EQ(lst_diver, expected_lst_diver);

    // Test with with convergence (increase MAXIT)
    KSIM_MAXIT = 100;
    sim.model_simulate(from, to);

    // Check result
    // exo
    EXPECT_DOUBLE_EQ(round(Variables.get_var("UY", "2000Y1") * 10e5) / 10e5, 624.177102);
    // endo
    EXPECT_DOUBLE_EQ(Variables.get_var("XNATY", "2000Y1"), 0.22);

    // --- exchange UY - XNATY ---
    // Set values of endo UY
    Variables.set_var("UY", "2000Y1", 650.0);
    Variables.set_var("UY", "2001Y1", 670.0);
    Variables.set_var("UY", "2002Y1", 680.0);

    sim.model_exchange(endo_exo);
    sim.model_simulate(from, to);

    // Check result
    EXPECT_DOUBLE_EQ(Variables.get_var("UY", "2000Y1"), 650.0);
    EXPECT_DOUBLE_EQ(round(Variables.get_var("XNATY", "2000Y1") * 10e5) / 10e5, 0.8007030);
    
    // TODO : check with list of equations
}

TEST_F(SimulationTest, CalculateSCC)
{
    // Invalid arguments
    // PRE list name empty
    EXPECT_THROW(sim.model_calculate_SCC(10, ""), IodeExceptionInvalidArguments);
    // invalid list of equations
    EXPECT_THROW(sim.model_calculate_SCC(10, "_PRE", "_INTER", "_POST", "$UNKNOWN_LIST"), IodeExceptionInvalidArguments);

    // SCC decomposition
    sim.model_calculate_SCC(10);

    std::string list_pre = Lists.get("_PRE");
    std::string expected_lst_pre = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;";
    expected_lst_pre += "PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    EXPECT_EQ(list_pre, expected_lst_pre);

    std::string list_inter = Lists.get("_INTER");
    std::string expected_lst_inter = "PMAB;PXAB;ULCP;SSH3P;WBG;ITF;EXCC;ITFQ;QS;ITFGO;ITFGI;CGU;SSH3O;DEBT;IDG;SSFFX;";
    expected_lst_inter += "SBF;SSF3;SBF3L;YDH;SUBCEE;SUB;RIDG;QOUG;OCUH;OCUG;IUG;ITPS;ITPR;ITPL;GOSH_;GOSG;DPUG;VAG;COTRES;";
    expected_lst_inter += "SSFG;WG;ACAG;FLG;IT;VAT;VAT_;ITMQ;PM;ITM;ITON;ITNQ;QAT_;QAFF_;KNF;QIF;KNFY;VAI;VAF;DTF;DTH;YDTG;";
    expected_lst_inter += "YSFIC;WMIN;WLCP;WBGP;YSEFT2;YSEFT1;YSEFP;SBG;SBH;PWBG;WBGO;SSFDOM;GOSF;BVY;VBBP_P;SH;ITFC;SSH;";
    expected_lst_inter += "RDEBT;YIDG;QAG;VAFF_;VAF_;VAFF;COEFON;ITONQ;QI;KN5;VI;ITF5;ITD;IDH;DPUGO;QG;QC;PAF_;VS_;VS;QAI_;";
    expected_lst_inter += "QAF_;QAI;NFY;VXN;VMN;YN;UY;NFYH;KLFHP;Q_F;QL;AOUC;QXAB;VXAB;PC;ZJ;WDOM;VXK;VMK;YK;VC;SSH3ZW;";
    expected_lst_inter += "SSH3GP;RSBE;RLBE;QXT;QMT;VMT;PXT;VXT;PXS;PQOG;QGO;PIG;QIG;PIF;VIF;PKF;PI5;VAH;QI5;VI5;PFI;";
    expected_lst_inter += "PDPUG;PC_;QC_;VC_;PFI_;WBF;WIND;WIND_;VAMARE;VAI_;SSH3W;SSH3WW;SSFFIC;SSF3P;SSF3L;SSFF;WCF;WCRH;";
    expected_lst_inter += "SSH3ZA;SSH3WA;SSF;SG;WBU_;YDH_;KNFF;KNI;KNIY;Q_I;QXS;QBBP_B;QBBP;QS_;QBBP_P;QMAB;VMAB;QM;VM;QX;";
    expected_lst_inter += "QQMAB_;QMS;VMS;QME;VME;QXE;VXE;VXS;VX;VBBP_B;VBNP_B;VBNP_I;VBBP;VBNP;VBNP_P;QAT;QAFF;QAF;PROD;W;";
    expected_lst_inter += "WBF_;YSSF;YSSG;WCF_;ITEP;EXC;ITT";
    EXPECT_EQ(list_inter, expected_lst_inter);

    std::string list_post = Lists.get("_POST");
    std::string expected_lst_post = "IFU;SSHFF;PBBP;OCUF;IHU;IDF;DPUH;DPUF;DPU;BQY;ACAF;ZF;WNF_;WNF;WBU;VXB;SF;RIPBE;RIDGG;";
    expected_lst_post += "RENT;QXB;QFND;QBNP;QBBPPOT_;PXB;PX;PG;PFND;PBNP;PAH;PAG;KNFFY;KL;GOSH;GAP;FLGR;FLF;DPUU;BENEF";
    EXPECT_EQ(list_post, expected_lst_post);

    // TODO : check with list of equations
}

TEST_F(SimulationTest, SimulateSCC)
{
    // Invalid arguments
    // invalid sample definition
    EXPECT_THROW(sim.model_simulate_SCC("2000U1", to), std::invalid_argument);
    // invalid pre-recursive list
    EXPECT_THROW(sim.model_simulate_SCC(from, to, "UNKNOWN_LIST"), IodeExceptionInvalidArguments);

    // SCC decomposition
    sim.model_calculate_SCC(10);

    // SCC simulation
    KSIM_MAXIT = 100;
    sim.model_simulate_SCC(from, to);

    // Check result
    // exo
    EXPECT_DOUBLE_EQ(round(Variables.get_var("UY", "2000Y1") * 10e5) / 10e5, 624.173844);
    // endo
    EXPECT_DOUBLE_EQ(Variables.get_var("XNATY", "2000Y1"), 0.22);

    // --- exchange UY - XNATY ---
    // Set values of endo UY
    Variables.set_var("UY", "2000Y1", 650.0);
    Variables.set_var("UY", "2001Y1", 670.0);
    Variables.set_var("UY", "2002Y1", 680.0);

    sim.model_exchange(endo_exo);
    sim.model_simulate_SCC(from, to);

    // Check result
    EXPECT_DOUBLE_EQ(round(Variables.get_var("UY", "2000Y1") * 10e5) / 10e5, 624.179951);
    EXPECT_DOUBLE_EQ(Variables.get_var("XNATY", "2000Y1"), 0.22);

    // TODO : check with list of equations
}
