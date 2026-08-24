#include "pch.h"


class SimulationTest : public TestAbstract, public ::testing::Test
{
protected:
    std::string from;
    std::string to;
    std::string endo_exo;

    void SetUp() override
    {
        global_ws_eqs->load(str_input_test_dir + "fun.ae");
        global_ws_scl->load(str_input_test_dir + "fun.as");
        global_ws_var->load(str_input_test_dir + "fun.av");

        from = "2000Y1";
        to = "2002Y1";

        endo_exo = "UY-XNATY";

        // Simulation std parameters
        global_simu->init_algo = VAR_INIT_TM1;
        global_simu->epsilon = 0.0001;
        global_simu->max_iter = 100;
        global_simu->relax = 0.7;
        global_simu->sorting_algo = SORT_BOTH;
        global_simu->nb_passes = 5;
        global_simu->debug = 1;
    }

    // void TearDown() override {}
};

TEST_F(SimulationTest, ModelExchange)
{
    bool success;

    success = global_simu->exchange(endo_exo);
    EXPECT_TRUE(success);
    EXPECT_EQ(global_simu->v_endo_exo.size(), 1);
    EXPECT_EQ(std::string(global_simu->v_endo_exo[0]), "UY-XNATY");

    // reset v_endo_exo
    success = global_simu->exchange();
    EXPECT_FALSE(success);
    EXPECT_TRUE(global_simu->v_endo_exo.empty());
}

TEST_F(SimulationTest, Simulation)
{
    bool success;

    global_simu->set_sort(SimuSortAlgorithm::SORT_NONE);
    global_simu->set_sort(SimuSortAlgorithm::SORT_CONNEX);
    global_simu->set_sort(SimuSortAlgorithm::SORT_BOTH);

    global_simu->set_initialization_method(VariablesInitialization::VAR_INIT_TM1);
    global_simu->set_initialization_method(VariablesInitialization::VAR_INIT_TM1_A);
    global_simu->set_initialization_method(VariablesInitialization::VAR_INIT_TM1_NA);
    global_simu->set_initialization_method(VariablesInitialization::VAR_INIT_ASIS);
    global_simu->set_initialization_method(VariablesInitialization::VAR_INIT_EXTRA);
    global_simu->set_initialization_method(VariablesInitialization::VAR_INIT_EXTRA_A);
    global_simu->set_initialization_method(VariablesInitialization::VAR_INIT_EXTRA_NA);

    global_simu->reset();

    // Invalid arguments
    // invalid sample definition
    success = global_simu->simulate("2000U1", to);
    EXPECT_FALSE(success);
    // invalid list of equations
    success = global_simu->simulate(from, to, "$UNKNOWN_LIST");
    EXPECT_FALSE(success);

    // Test simulation: divergence
    global_simu->max_iter = 2;
    success = global_simu->simulate(from, to);
    EXPECT_FALSE(success);

    // Check _DIVER list (divergent equations)
    std::string lst_diver = global_ws_lst->get("_DIVER");
    std::string expected_lst_diver = "WBG,YDH,QMT,QI5";
    EXPECT_EQ(lst_diver, expected_lst_diver);

    // Test with with convergence (increase MAXIT)
    global_simu->max_iter = 100;
    global_simu->set_debug(true);
    success = global_simu->simulate(from, to);
    EXPECT_TRUE(success);

    std::string list_pre = global_ws_lst->get("_PRE");
    std::string expected_lst_pre = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;";
    expected_lst_pre += "PWXS;PXAB;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PMAB;PME;PMS;PMT";
    EXPECT_EQ(list_pre, expected_lst_pre);

    std::string list_inter = global_ws_lst->get("_INTER");
    std::string expected_lst_inter = "ITFGO;ITFGI;CGU;SSH3O;ULCP;SSFFX;SBF;SBF3L;SSH3P;WBG;SSF3;NFY;VXN;VMN;YN;UY;WBF;WIND;";
    expected_lst_inter += "WIND_;VAMARE;SSFFIC;SSF3P;SSF3L;SSH3ZA;SSH3WA;KNF;QIF;KNFY;DEBT;IDG;DTF;VS;VAF;YDH;EXCC;EXC;ITF;";
    expected_lst_inter += "PAF_;QAI;QAF;ITEP;QAFF_;QAF_;QXS;QAFF;VAI;QAI_;IT;DTH;YDTG;YSFIC;WMIN;WLCP;WBGP;YSEFT2;YSEFT1;";
    expected_lst_inter += "YSEFP;SBG;SBH;PWBG;WBGO;WBU_;SSFDOM;SSH3W;SSFF;WCF;GOSF;BVY;SH;ITFC;SSH;RDEBT;YIDG;QAG;KNFF;KNI;";
    expected_lst_inter += "KNIY;Q_I;NFYH;WCRH;PROD;W;WBF_;YSSF;WCF_;KLFHP;Q_F;QL;VAFF_;VAF_;VS_;VAFF;COEFON;QI;KN5;ITF5;ITD;";
    expected_lst_inter += "IDH;QC;ITFQ;QS;ITONQ;ITMQ;ITNQ;QAT_;QAT;PM;VAI_;ITM;ITON;AOUC;QXAB;VXAB;PC;ZJ;WDOM;VXK;VMK;YK;VC;";
    expected_lst_inter += "SSH3ZW;SSH3GP;SSH3WW;RSBE;RLBE;QXT;QMT;VMT;PXT;VXT;PXS;VXS;PQOG;QGO;PIG;PIF;VIF;PKF;PI5;VAH;QI5;VI5;";
    expected_lst_inter += "PFI;PDPUG;PC_;QC_;VC_;PFI_;QBBP_B;QBBP;QS_;QBBP_P;QMAB;VMAB;QM;VM;QX;VX;VBBP_B;VBNP_B;VBNP_I;VBBP;";
    expected_lst_inter += "SUBCEE;SUB;VAT_;VAT;RIDG;QOUG;OCUH;OCUG;IUG;VI;QIG;ITPS;ITPR;ITPL;ITT;GOSH_;GOSG;DPUG;VAG;DPUGO;QG;";
    expected_lst_inter += "QQMAB_;QMS;VMS;QME;VME;QXE;VXE;COTRES;SSFG;WG;SSF;YSSG;YDH_;SG;ACAG;FLG;VBNP;VBNP_P;VBBP_P";
    EXPECT_EQ(list_inter, expected_lst_inter);

    std::string list_post = global_ws_lst->get("_POST");
    std::string expected_lst_post = "IFU;SSHFF;PBBP;OCUF;IHU;IDF;DPUH;DPUF;DPU;BQY;ACAF;ZF;WNF_;WNF;WBU;VXB;SF;RIPBE;RIDGG;";
    expected_lst_post += "RENT;QXB;QFND;QBNP;QBBPPOT_;PXB;PX;PG;PFND;PBNP;PAH;PAG;KNFFY;KL;GOSH;GAP;FLGR;FLF;DPUU;BENEF";
    EXPECT_EQ(list_post, expected_lst_post);

    // Check result
    // exo
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2000Y1") * 10e5) / 10e5, 624.186351);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2001Y1") * 10e5) / 10e5, 645.066228);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2002Y1") * 10e5) / 10e5, 661.628768);
    // endo
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2000Y1") * 10e3) / 10e3, 0.22);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2001Y1") * 10e3) / 10e3, 0.70);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2002Y1") * 10e3) / 10e3, 0.40);

    // --- exchange UY - XNATY ---
    // Set values of endo UY
    global_ws_var->set_var("UY", "2000Y1", 650.0);
    global_ws_var->set_var("UY", "2001Y1", 670.0);
    global_ws_var->set_var("UY", "2002Y1", 680.0);

    global_ws_var->set_var("XNATY", "2000Y1", 0.0);
    global_ws_var->set_var("XNATY", "2001Y1", 0.0);
    global_ws_var->set_var("XNATY", "2002Y1", 0.0);

    global_simu->reset();
    global_simu->set_debug(true);
    success = global_simu->exchange(endo_exo);
    EXPECT_TRUE(success);
    success = global_simu->simulate(from, to);
    EXPECT_TRUE(success);

    // exchange UY - XNATY -> NATY removed from block _PRE
    list_pre = global_ws_lst->get("_PRE");
    expected_lst_pre = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;POIL;PW3;PWMAB;PWMS;PWXAB;";
    expected_lst_pre += "PWXS;PXAB;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PMAB;PME;PMS;PMT";
    EXPECT_EQ(list_pre, expected_lst_pre);

    // exchange UY - XNATY -> replace UY by NATY in block _INTER
    list_inter = global_ws_lst->get("_INTER");
    expected_lst_inter = "ITFGO;ITFGI;CGU;SSH3O;ULCP;SSFFX;SBF;SBF3L;SSH3P;WBG;SSF3;NFY;VXN;VMN;YN;NATY;WBF;WIND;";
    expected_lst_inter += "WIND_;VAMARE;SSFFIC;SSF3P;SSF3L;SSH3ZA;SSH3WA;KNF;QIF;KNFY;DEBT;IDG;DTF;VS;VAF;YDH;EXCC;EXC;ITF;";
    expected_lst_inter += "PAF_;QAI;QAF;ITEP;QAFF_;QAF_;QXS;QAFF;VAI;QAI_;IT;DTH;YDTG;YSFIC;WMIN;WLCP;WBGP;YSEFT2;YSEFT1;";
    expected_lst_inter += "YSEFP;SBG;SBH;PWBG;WBGO;WBU_;SSFDOM;SSH3W;SSFF;WCF;GOSF;BVY;SH;ITFC;SSH;RDEBT;YIDG;QAG;KNFF;KNI;";
    expected_lst_inter += "KNIY;Q_I;NFYH;WCRH;PROD;W;WBF_;YSSF;WCF_;KLFHP;Q_F;QL;VAFF_;VAF_;VS_;VAFF;COEFON;QI;KN5;ITF5;ITD;";
    expected_lst_inter += "IDH;QC;ITFQ;QS;ITONQ;ITMQ;ITNQ;QAT_;QAT;PM;VAI_;ITM;ITON;AOUC;QXAB;VXAB;PC;ZJ;WDOM;VXK;VMK;YK;VC;";
    expected_lst_inter += "SSH3ZW;SSH3GP;SSH3WW;RSBE;RLBE;QXT;QMT;VMT;PXT;VXT;PXS;VXS;PQOG;QGO;PIG;PIF;VIF;PKF;PI5;VAH;QI5;VI5;";
    expected_lst_inter += "PFI;PDPUG;PC_;QC_;VC_;PFI_;QBBP_B;QBBP;QS_;QBBP_P;QMAB;VMAB;QM;VM;QX;VX;VBBP_B;VBNP_B;VBNP_I;VBBP;";
    expected_lst_inter += "SUBCEE;SUB;VAT_;VAT;RIDG;QOUG;OCUH;OCUG;IUG;VI;QIG;ITPS;ITPR;ITPL;ITT;GOSH_;GOSG;DPUG;VAG;DPUGO;QG;";
    expected_lst_inter += "QQMAB_;QMS;VMS;QME;VME;QXE;VXE;COTRES;SSFG;WG;SSF;YSSG;YDH_;SG;ACAG;FLG;VBNP;VBNP_P;VBBP_P";
    EXPECT_EQ(list_inter, expected_lst_inter);

    // exchange UY - XNATY -> XNATY added to block _POST
    list_post = global_ws_lst->get("_POST");
    expected_lst_post = "IFU;SSHFF;PBBP;OCUF;IHU;IDF;DPUH;DPUF;DPU;BQY;ACAF;ZF;WNF_;WNF;WBU;VXB;SF;RIPBE;RIDGG;RENT;";
    expected_lst_post += "QXB;QFND;QBNP;QBBPPOT_;PXB;PX;PG;PFND;PBNP;PAH;PAG;XNATY;KNFFY;KL;GOSH;GAP;FLGR;FLF;DPUU;BENEF";
    EXPECT_EQ(list_post, expected_lst_post);

    // Check result
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("UY", "2000Y1"), 650.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("UY", "2001Y1"), 670.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("UY", "2002Y1"), 680.0);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2000Y1") * 10e5) / 10e5, 0.801325);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2001Y1") * 10e5) / 10e5, 0.633894);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2002Y1") * 10e5) / 10e5, 0.395619);
}

TEST_F(SimulationTest, CalculateSCC)
{
    bool success;

    // Invalid arguments
    // PRE list name empty
    success = global_simu->calculate_SCC(10, "");
    EXPECT_FALSE(success);
    // invalid list of equations
    success = global_simu->calculate_SCC(10, "_PRE", "_INTER", "_POST", "$UNKNOWN_LIST");
    EXPECT_FALSE(success);

    // SCC decomposition
    success = global_simu->calculate_SCC(10);
    EXPECT_TRUE(success);

    std::string list_pre = global_ws_lst->get("_PRE");
    std::string expected_lst_pre = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;";
    expected_lst_pre += "PWXS;PXAB;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PMAB;PME;PMS;PMT";
    EXPECT_EQ(list_pre, expected_lst_pre);

    std::string list_inter = global_ws_lst->get("_INTER");
    std::string expected_lst_inter = "ULCP;SSH3P;WBG;ITF;EXCC;ITFQ;QS;ITFGO;ITFGI;CGU;SSH3O;DEBT;IDG;SSFFX;SBF;SSF3;SBF3L;";
    expected_lst_inter += "YDH;SUBCEE;SUB;RIDG;QOUG;OCUH;OCUG;IUG;ITPS;ITPR;ITPL;GOSH_;GOSG;DPUG;VAG;COTRES;SSFG;WG;ACAG;";
    expected_lst_inter += "FLG;IT;VAT;VAT_;ITMQ;PM;ITM;ITON;ITNQ;QAT_;QAFF_;KNF;QIF;KNFY;VAI;VAF;DTF;DTH;YDTG;YSFIC;WMIN;";
    expected_lst_inter += "WLCP;WBGP;YSEFT2;YSEFT1;YSEFP;SBG;SBH;PWBG;WBGO;SSFDOM;GOSF;BVY;VBBP_P;SH;ITFC;SSH;RDEBT;YIDG;";
    expected_lst_inter += "QAG;VAFF_;VAF_;VAFF;COEFON;ITONQ;QI;KN5;VI;ITF5;ITD;IDH;DPUGO;QG;QC;PAF_;VS_;VS;QAI_;QAF_;QAI;NFY";
    expected_lst_inter += ";VXN;VMN;YN;UY;NFYH;KLFHP;Q_F;QL;AOUC;QXAB;VXAB;PC;ZJ;WDOM;VXK;VMK;YK;VC;SSH3ZW;SSH3GP;RSBE;RLBE;";
    expected_lst_inter += "QXT;QMT;VMT;PXT;VXT;PXS;PQOG;QGO;PIG;QIG;PIF;VIF;PKF;PI5;VAH;QI5;VI5;PFI;PDPUG;PC_;QC_;VC_;PFI_;";
    expected_lst_inter += "WBF;WIND;WIND_;VAMARE;VAI_;SSH3W;SSH3WW;SSFFIC;SSF3P;SSF3L;SSFF;WCF;WCRH;SSH3ZA;SSH3WA;SSF;SG;WBU_;";
    expected_lst_inter += "YDH_;KNFF;KNI;KNIY;Q_I;QXS;QBBP_B;QBBP;QS_;QBBP_P;QMAB;VMAB;QM;VM;QX;QQMAB_;QMS;VMS;QME;VME;QXE;VXE;";
    expected_lst_inter += "VXS;VX;VBBP_B;VBNP_B;VBNP_I;VBBP;VBNP;VBNP_P;QAT;QAFF;QAF;PROD;W;WBF_;YSSF;YSSG;WCF_;ITEP;EXC;ITT";
    EXPECT_EQ(list_inter, expected_lst_inter);

    std::string list_post = global_ws_lst->get("_POST");
    std::string expected_lst_post = "IFU;SSHFF;PBBP;OCUF;IHU;IDF;DPUH;DPUF;DPU;BQY;ACAF;ZF;WNF_;WNF;WBU;VXB;SF;RIPBE;RIDGG;";
    expected_lst_post += "RENT;QXB;QFND;QBNP;QBBPPOT_;PXB;PX;PG;PFND;PBNP;PAH;PAG;KNFFY;KL;GOSH;GAP;FLGR;FLF;DPUU;BENEF";
    EXPECT_EQ(list_post, expected_lst_post);

    // TODO : check with list of equations
}

TEST_F(SimulationTest, SimulateSCC)
{
    bool success;

    // SCC decomposition
    success = global_simu->calculate_SCC(10);
    EXPECT_TRUE(success);

    // Invalid arguments
    // invalid sample definition
    success = global_simu->simulate_SCC("2000U1", to);
    EXPECT_FALSE(success);
    // invalid pre-recursive list
    success = global_simu->simulate_SCC(from, to, "UNKNOWN_LIST");
    EXPECT_FALSE(success);

    // SCC simulation
    global_simu->max_iter = 100;
    success = global_simu->simulate_SCC(from, to);
    EXPECT_TRUE(success);

    // Check result
    // exo
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2000Y1") * 10e5) / 10e5, 624.171926);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2001Y1") * 10e5) / 10e5, 645.049475);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2002Y1") * 10e5) / 10e5, 661.608054);
    // endo
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2000Y1") * 10e3) / 10e3, 0.22);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2001Y1") * 10e3) / 10e3, 0.70);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2002Y1") * 10e3) / 10e3, 0.40);

    // --- exchange UY - XNATY ---
    // Set values of endo UY
    global_ws_var->set_var("UY", "2000Y1", 650.0);
    global_ws_var->set_var("UY", "2001Y1", 670.0);
    global_ws_var->set_var("UY", "2002Y1", 680.0);

    global_ws_lst->remove("_PRE");
    global_ws_lst->remove("_INTER");
    global_ws_lst->remove("_POST");
    global_simu->reset();

    // set exchange --> not taken into account in calculate_SCC() or simulate_SCC() !
    success = global_simu->exchange(endo_exo);
    EXPECT_TRUE(success);
    // SCC decomposition
    success = global_simu->calculate_SCC(10);
    EXPECT_TRUE(success);
    // SCC simulation
    success = global_simu->simulate_SCC(from, to);
    EXPECT_TRUE(success);

    // Check result
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2000Y1") * 10e5) / 10e5, 624.18396);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2001Y1") * 10e5) / 10e5, 645.047397);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("UY", "2002Y1") * 10e5) / 10e5, 661.585134);

    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2000Y1") * 10e3) / 10e3, 0.22);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2001Y1") * 10e3) / 10e3, 0.70);
    EXPECT_DOUBLE_EQ(round(global_ws_var->get_var("XNATY", "2002Y1") * 10e3) / 10e3, 0.40);
}
