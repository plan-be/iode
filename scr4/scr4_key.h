

#ifndef _SCR_4_KEY_H_
#define _SCR_4_KEY_H_


/* =+============= SCR4_KEYS.H ============================================
    The keys are defined in 4 classes :

	SCR_VISIBLE : begin at 0
	SCR_EDIT    : begin at 1000
	SCR_ARROW   : begin at 2000
	SCR_FUNCTION: begin at 3000
	SCR_WAIT_KEY: begin at 4000

    The class of a key may be found be the macro SCR_key_class(int key),
    which returns one of the values VISIBLE, EDIT, ARROW, FUNCTION

    Each class is subdivided into 4 sub-classes :

	SCR_NORMAL  : unshifted key : offset 0
	SCR_SHIFT   : shifted key (except for VISIBLE KEYS) offset 100
	SCR_CONTROL : control keys  : offset 200
	SCR_ALT     : alternate keys : offset 300

    The shift state of a key may be found be the macro SCR_key_shift(int key),
    which returns one of the values NORMAL, SHIFT, CONTROL or ALT.

   ============================================================= */

#define SCR_key_class(x)   (1000 * (int)((x)/1000))
#define SCR_key_shift(x)   (100 * (int)(SCR_key_class(x)/100))

#define SCR_VISIBLE            0
#define SCR_EDIT            1000
#define SCR_ARROW           2000
#define SCR_FUNCTION        3000
#define SCR_WAIT_KEY        4000    /* BP_M 01-02-2012 13:10 */

#define SCR_IS_ALT(key)   (((key) >= SCR_A_A && (key) <= SCR_A_9) || ((key) >= SCR_A_F1 && (key) <= SCR_A_F20) || (key) == SCR_A_TAB || (key) == SCR_A_S_TAB) 
#define SCR_IS_CTL(key)   (((key) >= SCR_C_A && (key) <= SCR_C_9) || ((key) >= SCR_C_F1 && (key) <= SCR_C_F20) || (key) == SCR_C_TAB || (key) == SCR_C_S_TAB ((key) >= SCR_ARROW + 100 && (key) <= SCR_ARROW + 400)) 

/* =+============= EDIT KEYS ===================================
   ============================================================= */
#define SCR_INSERT          (SCR_EDIT+0)
#define SCR_DELETE          (SCR_EDIT+1)
#define SCR_ENTER           (SCR_EDIT+2)
#define SCR_TAB             9019
#define SCR_BACKSPACE       (SCR_EDIT+4)
#define SCR_LINEFEED        9024
#define SCR_ESCAPE          (SCR_EDIT+6)
#define SCR_ERASE_EOF       SCR_A_Z
#define SCR_REFRESH         (SCR_EDIT+8)
#define SCR_PRTSC           SCR_C_P
#define SCR_BREAK           (SCR_EDIT+10)
#define SCR_QUIT            SCR_A_Q
#define SCR_ERASE_FIELD     SCR_C_U
#define SCR_ZOOM            SCR_C_Z
#define SCR_SWITCH          SCR_A_E
#define SCR_CUT_KEY         SCR_A_D
#define SCR_COPY_KEY        SCR_A_C
#define SCR_MARK_KEY        SCR_A_L
#define SCR_SAVE_KEY        SCR_A_W
#define SCR_HELP_KEY        9025
#define SCR_CONFIG          SCR_C_A
#define SCR_NOTEPAD         SCR_C_N
#define SCR_QNOTEPAD        SCR_C_X
#define SCR_SEARCH          SCR_C_S
#define SCR_CHANGE          SCR_C_C

#define SCR_S_TAB           (SCR_EDIT+103)
#define SCR_C_TAB           (SCR_EDIT+250)  /* JMP 12-09-99 */
#define SCR_S_C_TAB         (SCR_EDIT+251)  /* JMP 12-09-99 */
#define SCR_C_S_TAB         SCR_S_C_TAB     /* JMP 12-09-99 */
#define SCR_A_TAB           (SCR_EDIT+252)  /* JMP 03-05-00 */
#define SCR_A_S_TAB         (SCR_EDIT+253)  /* JMP 03-05-00 */
#define SCR_S_A_TAB         SCR_A_S_TAB     /* JMP 03-05-00 */


#define SCR_C_ENTER         SCR_LINEFEED
#define SCR_C_BACKSPACE     (SCR_EDIT+237)

/* =+============= VISIBLE KEYS =================================
    These keys are defined as their ASCII value
   ============================================================= */

#define SCR_C_A             1200
#define SCR_C_B             1201
#define SCR_C_C             1202
#define SCR_C_D             1203
#define SCR_C_E             1204
#define SCR_C_F             1205
#define SCR_C_G             1206
#define SCR_C_H             SCR_BACKSPACE
#define SCR_C_I             SCR_TAB
#define SCR_C_J             SCR_C_ENTER
#define SCR_C_K             1210
#define SCR_C_L             1211
#define SCR_C_M             SCR_ENTER
#define SCR_C_N             1213
#define SCR_C_O             1214
#define SCR_C_P             1215
#define SCR_C_Q             1216
#define SCR_C_R             1217
#define SCR_C_S             1218
#define SCR_C_T             1219
#define SCR_C_U             1220
#define SCR_C_V             1221
#define SCR_C_W             1222
#define SCR_C_X             1223
#define SCR_C_Y             1224
#define SCR_C_Z             1225
#define SCR_C_0             1226
#define SCR_C_1             1227
#define SCR_C_2             1228
#define SCR_C_3             1229
#define SCR_C_4             1230
#define SCR_C_5             1231
#define SCR_C_6             1232
#define SCR_C_7             1233
#define SCR_C_8             1234
#define SCR_C_9             1235

#define SCR_A_A             1300
#define SCR_A_B             1301
#define SCR_A_C             1302
#define SCR_A_D             1303
#define SCR_A_E             1304
#define SCR_A_F             1305
#define SCR_A_G             1306
#define SCR_A_H             1307
#define SCR_A_I             1308
#define SCR_A_J             1309
#define SCR_A_K             1310
#define SCR_A_L             1311
#define SCR_A_M             1312
#define SCR_A_N             1313
#define SCR_A_O             1314
#define SCR_A_P             1315
#define SCR_A_Q             1316
#define SCR_A_R             1317
#define SCR_A_S             1318
#define SCR_A_T             1319
#define SCR_A_U             1320
#define SCR_A_V             1321
#define SCR_A_W             1322
#define SCR_A_X             1323
#define SCR_A_Y             1324
#define SCR_A_Z             1325
#define SCR_A_0             1326
#define SCR_A_1             1327
#define SCR_A_2             1328
#define SCR_A_3             1329
#define SCR_A_4             1330
#define SCR_A_5             1331
#define SCR_A_6             1332
#define SCR_A_7             1333
#define SCR_A_8             1334
#define SCR_A_9             1335

#define SCR_INSERT_1        1400
#define SCR_DELETE_1        1401
#define SCR_ENTER_1         1402
#define SCR_F1_1            1403
#define SCR_F2_1            1404
#define SCR_F3_1            1405
#define SCR_F4_1            1406
#define SCR_F5_1            1407
#define SCR_F6_1            1408
#define SCR_F7_1            1409
#define SCR_F8_1            1410
#define SCR_F9_1            1411
#define SCR_F10_1           1412
#define SCR_ESCAPE_1        1413

/* =+============= ARROW KEYS ===================================
   ============================================================= */
#define SCR_CSR_UP          (SCR_ARROW+0)
#define SCR_CSR_DOWN        (SCR_ARROW+1)
#define SCR_CSR_LEFT        (SCR_ARROW+2)
#define SCR_CSR_RIGHT       (SCR_ARROW+3)
#define SCR_CSR_PG_UP       (SCR_ARROW+4)
#define SCR_CSR_PG_DN       (SCR_ARROW+5)
#define SCR_CSR_HOME        (SCR_ARROW+6)
#define SCR_CSR_END         (SCR_ARROW+7)
#define SCR_WHEEL_UP        (SCR_ARROW+10)
#define SCR_WHEEL_DOWN      (SCR_ARROW+11)

#define SCR_S_CSR_UP        (SCR_ARROW+100)
#define SCR_S_CSR_DOWN      (SCR_ARROW+101)
#define SCR_S_CSR_LEFT      (SCR_ARROW+102)
#define SCR_S_CSR_RIGHT     (SCR_ARROW+103)
#define SCR_S_CSR_PG_UP     (SCR_ARROW+104)
#define SCR_S_CSR_PG_DN     (SCR_ARROW+105)
#define SCR_S_CSR_HOME      (SCR_ARROW+106)
#define SCR_S_CSR_END       (SCR_ARROW+107)
#define SCR_S_WHEEL_UP      (SCR_ARROW+110)
#define SCR_S_WHEEL_DOWN    (SCR_ARROW+111)

#define SCR_C_CSR_UP        (SCR_ARROW+200)
#define SCR_C_CSR_DOWN      (SCR_ARROW+201)
#define SCR_C_CSR_LEFT      (SCR_ARROW+202)
#define SCR_C_CSR_RIGHT     (SCR_ARROW+203)
#define SCR_C_CSR_PG_UP     (SCR_ARROW+204)
#define SCR_C_CSR_PG_DN     (SCR_ARROW+205)
#define SCR_C_CSR_HOME      (SCR_ARROW+206)
#define SCR_C_CSR_END       (SCR_ARROW+207)
#define SCR_C_WHEEL_UP      (SCR_ARROW+210)
#define SCR_C_WHEEL_DOWN    (SCR_ARROW+211)

#define SCR_C_S_CSR_UP        (SCR_ARROW+300)
#define SCR_C_S_CSR_DOWN      (SCR_ARROW+301)
#define SCR_C_S_CSR_LEFT      (SCR_ARROW+302)
#define SCR_C_S_CSR_RIGHT     (SCR_ARROW+303)
#define SCR_C_S_CSR_PG_UP     (SCR_ARROW+304)
#define SCR_C_S_CSR_PG_DN     (SCR_ARROW+305)
#define SCR_C_S_CSR_HOME      (SCR_ARROW+306)
#define SCR_C_S_CSR_END       (SCR_ARROW+307)
#define SCR_C_S_WHEEL_UP      (SCR_ARROW+310)
#define SCR_C_S_WHEEL_DOWN    (SCR_ARROW+311)

#define SCR_MOVE_KEY        (SCR_ARROW+208)

#define SCR_CSR_UP_1        (SCR_ARROW+500)
#define SCR_CSR_DOWN_1      (SCR_ARROW+501)
#define SCR_CSR_LEFT_1      (SCR_ARROW+502)
#define SCR_CSR_RIGHT_1     (SCR_ARROW+503)
#define SCR_CSR_PG_UP_1     (SCR_ARROW+504)
#define SCR_CSR_PG_DN_1     (SCR_ARROW+505)
#define SCR_CSR_HOME_1      (SCR_ARROW+506)
#define SCR_CSR_END_1       (SCR_ARROW+507)
#define SCR_WHEEL_UP_1      (SCR_ARROW+510)
#define SCR_WHEEL_DOWN_1    (SCR_ARROW+511)


/* =+============= FUNCTION KEYS ===================================
    Begin at 3000
   ============================================================= */
#define SCR_F1              (SCR_FUNCTION+1 )
#define SCR_F2              (SCR_FUNCTION+2 )
#define SCR_F3              (SCR_FUNCTION+3 )
#define SCR_F4              (SCR_FUNCTION+4 )
#define SCR_F5              (SCR_FUNCTION+5 )
#define SCR_F6              (SCR_FUNCTION+6 )
#define SCR_F7              (SCR_FUNCTION+7 )
#define SCR_F8              (SCR_FUNCTION+8 )
#define SCR_F9              (SCR_FUNCTION+9 )
#define SCR_F10             (SCR_FUNCTION+10)
#define SCR_F11             (SCR_FUNCTION+11)
#define SCR_F12             (SCR_FUNCTION+12)
#define SCR_F13             (SCR_FUNCTION+13)
#define SCR_F14             (SCR_FUNCTION+14)
#define SCR_F15             (SCR_FUNCTION+15)
#define SCR_F16             (SCR_FUNCTION+16)
#define SCR_F17             (SCR_FUNCTION+17)
#define SCR_F18             (SCR_FUNCTION+18)
#define SCR_F19             (SCR_FUNCTION+19)
#define SCR_F20             (SCR_FUNCTION+20)

#define SCR_S_F1            (SCR_FUNCTION+101)
#define SCR_S_F2            (SCR_FUNCTION+102)
#define SCR_S_F3            (SCR_FUNCTION+103)
#define SCR_S_F4            (SCR_FUNCTION+104)
#define SCR_S_F5            (SCR_FUNCTION+105)
#define SCR_S_F6            (SCR_FUNCTION+106)
#define SCR_S_F7            (SCR_FUNCTION+107)
#define SCR_S_F8            (SCR_FUNCTION+108)
#define SCR_S_F9            (SCR_FUNCTION+109)
#define SCR_S_F10           (SCR_FUNCTION+110)
#define SCR_S_F11           (SCR_FUNCTION+111)
#define SCR_S_F12           (SCR_FUNCTION+112)
#define SCR_S_F13           (SCR_FUNCTION+113)
#define SCR_S_F14           (SCR_FUNCTION+114)
#define SCR_S_F15           (SCR_FUNCTION+115)
#define SCR_S_F16           (SCR_FUNCTION+116)
#define SCR_S_F17           (SCR_FUNCTION+117)
#define SCR_S_F18           (SCR_FUNCTION+118)
#define SCR_S_F19           (SCR_FUNCTION+119)
#define SCR_S_F20           (SCR_FUNCTION+120)

#define SCR_C_F1            (SCR_FUNCTION+201)
#define SCR_C_F2            (SCR_FUNCTION+202)
#define SCR_C_F3            (SCR_FUNCTION+203)
#define SCR_C_F4            (SCR_FUNCTION+204)
#define SCR_C_F5            (SCR_FUNCTION+205)
#define SCR_C_F6            (SCR_FUNCTION+206)
#define SCR_C_F7            (SCR_FUNCTION+207)
#define SCR_C_F8            (SCR_FUNCTION+208)
#define SCR_C_F9            (SCR_FUNCTION+209)
#define SCR_C_F10           (SCR_FUNCTION+210)
#define SCR_C_F11           (SCR_FUNCTION+211)
#define SCR_C_F12           (SCR_FUNCTION+212)
#define SCR_C_F13           (SCR_FUNCTION+213)
#define SCR_C_F14           (SCR_FUNCTION+214)
#define SCR_C_F15           (SCR_FUNCTION+215)
#define SCR_C_F16           (SCR_FUNCTION+216)
#define SCR_C_F17           (SCR_FUNCTION+217)
#define SCR_C_F18           (SCR_FUNCTION+218)
#define SCR_C_F19           (SCR_FUNCTION+219)
#define SCR_C_F20           (SCR_FUNCTION+220)

#define SCR_A_F1            (SCR_FUNCTION+301)
#define SCR_A_F2            (SCR_FUNCTION+302)
#define SCR_A_F3            (SCR_FUNCTION+303)
#define SCR_A_F4            (SCR_FUNCTION+304)
#define SCR_A_F5            (SCR_FUNCTION+305)
#define SCR_A_F6            (SCR_FUNCTION+306)
#define SCR_A_F7            (SCR_FUNCTION+307)
#define SCR_A_F8            (SCR_FUNCTION+308)
#define SCR_A_F9            (SCR_FUNCTION+309)
#define SCR_A_F10           (SCR_FUNCTION+310)
#define SCR_A_F11           (SCR_FUNCTION+311)
#define SCR_A_F12           (SCR_FUNCTION+312)
#define SCR_A_F13           (SCR_FUNCTION+313)
#define SCR_A_F14           (SCR_FUNCTION+314)
#define SCR_A_F15           (SCR_FUNCTION+315)
#define SCR_A_F16           (SCR_FUNCTION+316)
#define SCR_A_F17           (SCR_FUNCTION+317)
#define SCR_A_F18           (SCR_FUNCTION+318)
#define SCR_A_F19           (SCR_FUNCTION+319)
#define SCR_A_F20           (SCR_FUNCTION+320)

/* BP_M 01-02-2012 13:09 */
#define SCR_WAIT_KEY_1      (SCR_WAIT_KEY+1)
#define SCR_WAIT_KEY_10     (SCR_WAIT_KEY+10)
#define SCR_WAIT_KEY_100    (SCR_WAIT_KEY+100)
#define SCR_WAIT_KEY_1000   (SCR_WAIT_KEY+1000)


/*  === WYSE compatibility =================== */

#define SCR_DO              SCR_F10
#define SCR_HELP            SCR_F1
#define SCR_FIND            SCR_F10
#define SCR_SELECT          SCR_F10
#define SCR_REMOVE          SCR_F10
#define SCR_PF1             SCR_F1
#define SCR_PF2             SCR_F2
#define SCR_PF3             SCR_F3
#define SCR_PF4             SCR_F4

/* =+============= MOUSE FUNCTIONS =============================
   ============================================================= */
extern int SCR_MS_X;
extern int SCR_MS_Y;
extern int SCR_ms_button;
extern int SCR_ms_event;
#define SCR_MOUSE               20000
#define SCR_MS_LEFT             0
#define SCR_MS_MIDDLE           1
#define SCR_MS_RIGHT            2
#define SCR_LOC_DRAG            10
#define SCR_LOC_PUSH            11
#define SCR_LOC_RELEASE         12


#endif /* _SCR_4_KEY_H_ */
