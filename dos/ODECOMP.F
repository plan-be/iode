
PAGE vkip_exec {
    Title "Execute Identities"
    $PGHEAD
    END_FN {C_FN return(C_IdtExecute());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ            E X E C U T I O N    O F   I D E N T I T I E S                 บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออออน
บ Execute From     $from        To $to                      ณ &F10          บ
บ                                                           ณ               บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด               บ
บ Identities list ณ$idts                                    ณ &ESC          บ
บ (empty = all)   ณ ณ                                       ณ               บ
บ                 ณ ณ                                       ณ &F1           บ
ฬอออออออออออออออออุอุอออออออออออออออออออออออออออออออออออออออต               บ
บ Input Var.Files ณ$varfiles                                ณ               บ
บ                 ณ ณ                                       ณ               บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด               บ
บ Input Scal.Filesณ$sclfiles                                ณ               บ
บ                 ณ ณ                                       ณ               บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Trace ? $trace           Lang $lang                                       บ
บ                                                                           บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD "1970Y1  " nc 10 allowed "0123456789YSQM " upper name from $FLDBOX
    FIELD "1980Y1  " nc 10 allowed "0123456789YSQM " upper name to   $FLDBOX
    FIELD nc 41 nl 3  name idts EDITOR SBAR

    FIELD nc 41 nl 2 EDITOR name varfiles sbar
    FIELD nc 41 nl 2 EDITOR name sclfiles sbar

    FIELD NAME trace MENU vkm_ny    $FLDBOX
    FIELD NAME lang  MENU vkm_lang  $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   Ok   "
}

PAGE vkip_compile {
    Title "Compile Model"
    $PGHEAD
    END_FN {C_FN return(C_ModelCompile());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                    C O M P I L E   M O D E L                         บ
ฬอออออออออออออออออัออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Equation list   ณ$eqs                                    ณ &F10      บ
บ (empty = all)   ณ ณ                                      ณ           บ
บ                 ณ ณ                                      ณ &ESC      บ
บ                 ณ ณ                                      ณ           บ
บ                 ณ ณ                                      ณ &F1       บ
ฬอออออออออออออออออฯอฯออออออออออออออออออออออออออออออออออออออต           บ
บ Recompiles the given set of equations using the lists    ณ           บ
บ defined in WS. This can be necessary if you use lists    ณ           บ
บ in your equation definitions and you load another file   ณ           บ
บ of lists                                                 ณ           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD nc 40 nl 5  name eqs EDITOR SBAR blue

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   Ok   "
}

PAGE vkip_sim {
    Title "Model Simulation"
    $PGHEAD
    FNKEY  {
	F1             "Help"
	ESCAPE {ABORT} "Quit"
	F3     {ABORT} "Quit"
	F10 {C_FN C_ModelSimulate();}   "Simulate"
    }
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                   M O D E L   S I M U L A T I O N                        บ
ฬออออออออออออออออัอัอออออออออออออออออออออออออออออออออออออออออออออัอออออออออน
บ Equations list ณ$eqs                                           ณ&F10     บ
บ (empty=>all)   ณ ณ                                             ณ         บ
บ                ณ ณ                                             ณ         บ
บ                ณ ณ                                             ณ         บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด         บ
บ Exchange(EN-EX)ณ$exo                                           ณ&ESC     บ
บ                ณ ณ                                             ณ         บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด         บ
บ Simulate From   $from       To $to       Convergence $eps      ณ         บ
บ                                                                ณ         บ
บ Max.iterations  $maxit   Debug $debug    Relaxation  $relax    ณ&F1      บ
บ                                                                ณ         บ
บ Initialisation  $start                                         ณ         บ
บ                                                                ณ         บ
บ Sort algorithm  $sort                            Passes $passesณ         บ
บ                                                                ณ         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออผ
}

    FIELD name from   "2010Y1  " allowed "0123456789YySsQqMm "  upper $FLDBOX  required
    FIELD name to     "2020Y1  " allowed "0123456789YySsQqMm "  upper $FLDBOX  required
    FIELD name eqs nc 47 nl 4 upper EDITOR SBAR                    blue
    FIELD name exo nc 47 nl 2 upper EDITOR SBAR                    blue

    FIELD name eps    "0.001   " real                              $FLDBOX     required
    FIELD name maxit  "100     " natural                           $FLDBOX     required
    FIELD name relax  "1.0     " real                              $FLDBOX     required
    FIELD name debug             MENU vkm_ny                       $FLDBOX
    FIELD name sort              MENU {
				    TITLE "SORTING"
				    OPTION "Connex compon. decomposition"
				    OPTION "Connex compon. + Triangulation"
				    OPTION "None"
				    }                              $FLDBOX
    FIELD name start             MENU vkm_extrap                   $FLDBOX
    FIELD name passes "5 "       natural                           $FLDBOX     required

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

/*
PAGE vkp_reschk {
    Center Back Cyan PROFILE
    END_FN {C_FN return(ODE_reschk());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                    R E S I D U A L    C H E C K                          บ
ฬออออออออออออออออออออออัอออออออออออออออออออออออออออออออออออออออออออออออออออน
บ Sample         from  ณ $from                                             บ
บ                to    ณ $to                                               บ
บ Error tolerance in % ณ $error                                            บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Equation list        ณ$eqs                                               บ
บ (Empty list = all)   ณ ณ                                                 บ
บ                      ณ ณ                                                 บ
ฬออออออออออออออออออออออุอฯอออออออออออออออออออออออออออออออออออออออออออออออออน
บ Result list          ณ $out                                              บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Result               ณ *res                                              บ
บ                      ณ                                                   บ
บ                      ณ                                                   บ
ฬออออออออออออออออออออออฯอออออออออออออออออออออออออออออออออออออออออออออออออออน
บ The residual check program computes all equations. It compares the       บ
บ results with the current WS value of the endogenous variable. If         บ
บ the difference exceeds tolerance, the equation is indicated.             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD name from  nc 8 allowed "0123456789YSQM " upper
    FIELD name to    nc 8 allowed "0123456789YSQM " upper
    FIELD name error real "1e-3       " nb_dec 6
    FIELD name eqs   nc 51 nl 3 EDITOR SBAR
    FIELD name out   nc 10 upper
    FIELD name res   nc 50 nl 3 upper
}
*/


PAGE vkip_calcscc {
    Title "SCC Decomposition "
    $PGHEAD
    FNKEY  {
	F1             "Help"
	ESCAPE {ABORT} "Quit"
	F3     {ABORT} "Quit"
	F10 {C_FN C_ModelCalcSCC();}   "Generate"
    }
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                   S C C   D E C O M P O S I T I O N                      บ
ฬออออออออออออออออัอัอออออออออออออออออออออออออออออออออออออออออออออัอออออออออน
บ Equations list ณ$eqs                                           ณ&F10     บ
บ (empty=>all)   ณ ณ                                             ณ         บ
บ                ณ ณ                                             ณ&ESC     บ
บ                ณ ณ                                             ณ         บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด         บ
บ Triangulation iterations : $passes                             ณ&F1      บ
บ                                                                ณ         บ
บ Save results in lists                                          ณ         บ
บ ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ                                          ณ         บ
บ Pre-recursive list name  : $pre                                ณ         บ
บ                                                                ณ         บ
บ Recursive list name      : $inter                              ณ         บ
บ                                                                ณ         บ
บ Post-recursive list name : $post                               ณ         บ
บ                                                                ณ         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออผ
}

    FIELD name eqs nc 47 nl 4 upper EDITOR SBAR                    blue
    FIELD name passes "5 "       natural                           $FLDBOX
    FIELD name pre   nc 15 "_PRE"   required upper sqz             $FLDBOX
    FIELD name inter nc 15 "_INTER" required upper sqz             $FLDBOX
    FIELD name post  nc 15 "_POST " required upper sqz             $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkip_simscc {
    Title "SCC Simulation"
    $PGHEAD
    FNKEY  {
	F1             "Help"
	ESCAPE {ABORT} "Quit"
	F3     {ABORT} "Quit"
	F10 {C_FN C_ModelSimulateSCC();}   "Simulate"
    }
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                       S C C   S I M U L A T I O N                        บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออน
บ Pre-recursive block (list name)  : $pre                        ณ         บ
บ                                                                ณ         บ
บ Recursive block (list name)      : $inter                      ณ&F10     บ
บ                                                                ณ         บ
บ Post-recursive block (list name) : $post                       ณ&ESC     บ
บ                                                                ณ         บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด         บ
บ Simulation : From $from      To $to                            ณ         บ
บ                                                                ณ         บ
บ Initial values :  $start                                       ณ         บ
บ                                                                ณ         บ
บ Relaxation :      $relax                                       ณ         บ
บ                                                                ณ         บ
บ Convergence :     $eps      Maxit : $maxit                     ณ         บ
บ                                                                ณ         บ
บ Debug $debug                                                   ณ&F1      บ
บ                                                                ณ         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออผ
}

    FIELD name pre   nc 15 "_PRE"   required upper sqz $FLDBOX
    FIELD name inter nc 15 "_INTER" required upper sqz $FLDBOX
    FIELD name post  nc 15 "_POST " required upper sqz $FLDBOX

    FIELD name from   "2010Y1  " allowed "0123456789YySsQqMm "  upper $FLDBOX required
    FIELD name to     "2020Y1  " allowed "0123456789YySsQqMm "  upper $FLDBOX required
    FIELD name eps    "0.001   " real                              $FLDBOX    required
    FIELD name maxit  "100     " natural                           $FLDBOX    required
    FIELD name relax  "1.0     " real                              $FLDBOX    required
    FIELD name debug             MENU vkm_ny                       $FLDBOX
    FIELD name start             MENU vkm_extrap                   $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

