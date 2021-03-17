/* ======== VARIABLES INTERVENANT DANS LES TEXTES DANS 3 LANGUES ==== */

char   *KLG_TYPE[][3] = {
    "Comments",     "Commentaires", "Commentaar",
    "Equations",    "Equations",    "Vergelijkingen",
    "Identities",   "Identit‚s",    "Identiteiten",
    "Lists",        "Listes",       "Lijsten",
    "Scalars",      "Scalaires",    "Scalairen",
    "Tables",       "Tableaux",     "Tabellen",
    "Variables",    "Variables",    "Variabelen"
};

char    *KLG_MONTHS[][3] = {
    "January",      "Janvier",      "Januari",
    "February",     "F‚vrier",      "Februari",
    "March",        "Mars",         "Maart",
    "April",        "Avril",        "April",
    "May",          "Mai",          "Mei",
    "June",         "Juin",         "Juni",
    "July",         "Juillet",      "Juli",
    "August",       "Ao–t",         "Augustus",
    "September",    "Septembre",    "September",
    "October",      "Octobre",      "Oktober",
    "November",     "Novembre",     "November",
    "December",     "Decembre",     "December"
};

char    *KLG_ROM[] = {
    "I",    "II",   "III",  "IV",   "V",    "VI",
    "VII",  "VIII", "IX",   "X",    "XI",   "XII"
};

char   KLG_E_LEVEL[] = "Level";
char   KLG_F_LEVEL[] = "Niveaux";
char   KLG_N_LEVEL[] = "Niveau";

char   KLG_E_DIFF[] = "Differences";
char   KLG_F_DIFF[] = "Diff‚rences";
char   KLG_N_DIFF[] = "Verschil";

char   KLG_E_GRT[] = "Growth Rates";
char   KLG_F_GRT[] = "Taux de croissance";
char   KLG_N_GRT[] = "Groeivoeten";

char   KLG_E_DIFFY[] = "YoY Differences";
char   KLG_F_DIFFY[] = "Diff‚rences Annueles";
char   KLG_N_DIFFY[] = "JoJ-Verschil";

char   KLG_E_GRTY[] = "YoY Growth Rates";
char   KLG_F_GRTY[] = "Taux de croissance Annuels";
char   KLG_N_GRTY[] = "JoJ-Groeivoeten";

char   KLG_E_OBS[] = "Observed";
char   KLG_F_OBS[] = "Observ‚";
char   KLG_N_OBS[] = "Gegeven";

char   KLG_E_CALC[] = "Computed";
char   KLG_F_CALC[] = "Calcul‚";
char   KLG_N_CALC[] = "Geschat";

char   KLG_E_RES[] = "Residuals";
char   KLG_F_RES[] = "R‚sidus";
char   KLG_N_RES[] = "Residus";

char   *KLG_CALC[3] = {KLG_E_CALC, KLG_F_CALC, KLG_N_CALC};
char   *KLG_OBS[3]  = {KLG_E_OBS,  KLG_F_OBS,  KLG_N_OBS};
char   *KLG_RES[3]  = {KLG_E_RES,  KLG_F_RES,  KLG_N_RES};

char   *KLG_MODES[5][3] = {
	KLG_E_LEVEL, KLG_F_LEVEL, KLG_N_LEVEL,
	KLG_E_DIFF,  KLG_F_DIFF,  KLG_N_DIFF,
	KLG_E_GRT,   KLG_F_GRT,   KLG_N_GRT,
	KLG_E_DIFFY, KLG_F_DIFFY, KLG_N_DIFFY,
	KLG_E_GRTY,  KLG_F_GRTY,  KLG_N_GRTY
};


char   *KLG_OPERS_TEXTS[][3] = {    /* JMP38 01-10-92 */
    KLG_E_LEVEL,            KLG_F_LEVEL,                  KLG_N_LEVEL,
    KLG_E_DIFF,             KLG_F_DIFF,                   KLG_N_DIFF,
    "Average Differences",  "Diff‚rences moyennes",       "Gemiddeld verschil",
    "Period average",       "Moyennes de p‚riode",        "Gemiddelden van periodes",
    KLG_E_GRT,              KLG_F_GRT,                    KLG_N_GRT,
    "Average Growth Rates", "Taux de croissance moyens",  "Gemiddelde groeivoeten", // JMP 14/12/2016
    "Sum of periods",       "Sommes de p‚riodes",         "Som van periodes",
    "Base year",            "En base",                    "Basis jaar"
};









