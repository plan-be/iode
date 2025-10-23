#pragma once

/**
 * Variables containing translations in 3 languages used in estimation graphs, calculated tables and graphs.
 *
 *      int     K_LANG                  Current language (1-3, 0=English, 1=French, 2=Dutch)
 *      char   *KLG_TYPE[][3]           Texts describing object types
 *      char   *KLG_MONTHS[][3]         Texts representing Months
 *      char   *KLG_ROM[]               Roman numerals from 1 to 12
 *      char   *KLG_CALC[3]             Legends for computed values line in estimation graphs
 *      char   *KLG_OBS[3]              Legends for observed values line in estimation graphs
 *      char   *KLG_RES[3]              Legends for residuals in estimation graphs
 *      char   *KLG_MODES[5][3]         Display modes for variables in computed Tables and Graphs    
 *      char   *KLG_OPERS_TEXTS[][3]    Texts representing operations on files and periods in computed tables and graphs
 */


// Current Language
inline int     K_LANG = 0;         // 0=English, 1=French, 2=Dutch

// Object types
inline char   *KLG_TYPE[][3] = {
    "Comments",     "Commentaires", "Commentaar",
    "Equations",    "Equations",    "Vergelijkingen",
    "Identities",   "Identités",    "Identiteiten",
    "Lists",        "Listes",       "Lijsten",
    "Scalars",      "Scalaires",    "Scalairen",
    "Tables",       "Tableaux",     "Tabellen",
    "Variables",    "Variables",    "Variabelen"
};

// Months
inline char    *KLG_MONTHS[][3] = {
    "January",      "Janvier",      "Januari",
    "February",     "Février",      "Februari",
    "March",        "Mars",         "Maart",
    "April",        "Avril",        "April",
    "May",          "Mai",          "Mei",
    "June",         "Juin",         "Juni",
    "July",         "Juillet",      "Juli",
    "August",       "Août",         "Augustus",
    "September",    "Septembre",    "September",
    "October",      "Octobre",      "Oktober",
    "November",     "Novembre",     "November",
    "December",     "Decembre",     "December"
};

// Roman numerals from 1 to 12
inline char    *KLG_ROM[] = {
    "I",    "II",   "III",  "IV",   "V",    "VI",
    "VII",  "VIII", "IX",   "X",    "XI",   "XII"
};

inline char   KLG_E_LEVEL[] = "Level";
inline char   KLG_F_LEVEL[] = "Niveaux";
inline char   KLG_N_LEVEL[] = "Niveau";

inline char   KLG_E_DIFF[] = "Differences";
inline char   KLG_F_DIFF[] = "Différences";
inline char   KLG_N_DIFF[] = "Verschil";

inline char   KLG_E_GRT[] = "Growth Rates";
inline char   KLG_F_GRT[] = "Taux de croissance";
inline char   KLG_N_GRT[] = "Groeivoeten";

inline char   KLG_E_DIFFY[] = "YoY Differences";
inline char   KLG_F_DIFFY[] = "Différences Annueles";
inline char   KLG_N_DIFFY[] = "JoJ-Verschil";

inline char   KLG_E_GRTY[] = "YoY Growth Rates";
inline char   KLG_F_GRTY[] = "Taux de croissance Annuels";
inline char   KLG_N_GRTY[] = "JoJ-Groeivoeten";

inline char   KLG_E_OBS[] = "Observed";
inline char   KLG_F_OBS[] = "Observé";
inline char   KLG_N_OBS[] = "Gegeven";

inline char   KLG_E_CALC[] = "Computed";
inline char   KLG_F_CALC[] = "Calculé";
inline char   KLG_N_CALC[] = "Geschat";

inline char   KLG_E_RES[] = "Residuals";
inline char   KLG_F_RES[] = "Résidus";
inline char   KLG_N_RES[] = "Residus";

// Legend for computed values line in estimation graphs
inline char   *KLG_CALC[3] = {KLG_E_CALC, KLG_F_CALC, KLG_N_CALC};

// Legend for observed values line in estimation graphs
inline char   *KLG_OBS[3]  = {KLG_E_OBS,  KLG_F_OBS,  KLG_N_OBS};

// Legend for residuals in estimation graphs
inline char   *KLG_RES[3]  = {KLG_E_RES,  KLG_F_RES,  KLG_N_RES};

// Display modes for variables in computed Tables and Graphs
inline char   *KLG_MODES[5][3] = {
    KLG_E_LEVEL, KLG_F_LEVEL, KLG_N_LEVEL,
    KLG_E_DIFF,  KLG_F_DIFF,  KLG_N_DIFF,
    KLG_E_GRT,   KLG_F_GRT,   KLG_N_GRT,
    KLG_E_DIFFY, KLG_F_DIFFY, KLG_N_DIFFY,
    KLG_E_GRTY,  KLG_F_GRTY,  KLG_N_GRTY
};

// Texts representing operations on files and periods in computed tables and graphs
inline char   *KLG_OPERS_TEXTS[][3] = {    /* JMP38 01-10-92 */
    KLG_E_LEVEL,            KLG_F_LEVEL,                  KLG_N_LEVEL,
    KLG_E_DIFF,             KLG_F_DIFF,                   KLG_N_DIFF,
    "Average Differences",  "Différences moyennes",       "Gemiddeld verschil",
    "Period average",       "Moyennes de période",        "Gemiddelden van periodes",
    KLG_E_GRT,              KLG_F_GRT,                    KLG_N_GRT,
    "Average Growth Rates", "Taux de croissance moyens", "Gemiddelde groeivoeten",
    "Sum of periods",       "Sommes de périodes",         "Som van periodes",
    "Base year",            "En base",                    "Basis jaar"
};
