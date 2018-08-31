/**
 * @file
 * scaler_time.c 
 * @author Brian Kang
 * @version 1.0
 * @section LICENSE
 *	Copyright (C) 2011~2012 Intersil Corporation
 * @section DESCRIPTION
 *	video scaler time table 
 *
 */
/*
Disclaimer: THIS INFORMATION IS PROVIDED 'AS-IS' FOR EVALUATION PURPOSES ONLY.  
INTERSIL CORPORATION AND ITS SUBSIDIARIES ('INTERSIL') DISCLAIM ALL WARRANTIES, 
INCLUDING WITHOUT LIMITATION FITNESS FOR A PARTICULAR PURPOSE AND MERCHANTABILITY.  
Intersil provides evaluation platforms to help our customers to develop products. 
However, factors beyond Intersil's control could significantly affect Intersil 
product performance. 
It remains the customers' responsibility to verify the actual system performance.
*/

#if 1
code struct DEC_VIDEO_TIME_TABLE_s TW8836_DEC_TABLE[] = {
/*0*/	{720,240,60,	9,21,0, 	3,2, 	0},	   	//NTSC
/*1*/	{720,288,50,	9,24,40, 	3,2, 	1},	   	//PAL
/*2*/	{720,288,50,	5,23,40, 	3,2, 	0},		//SECAM
/*3*/	{720,240,60,	9,21,0, 	3,2, 	0},		//NTSC4.43
/*4*/	{720,240,60,	9,21,0, 	3,2, 	0},		//PAL (M)
/*5*/	{720,288,50,	5,23,40, 	3,2, 	0},		//PAL (CN)
/*6*/	{720,240,60,	9,21,0, 	3,2, 	0}		//PAL 60
};
//NTSC DE 1440,262,60,   ?,1,274   263,0,1
//PAL  DE 1440,288,50,   ?,1,286,  313,0,1
#else
//code struct DEC_VIDEO_TIME_TABLE_s TW8836_DEC_TABLE[] = {
///*0*/	{720,240,60,	9,21,0 	3,2, 	0},	   	//NTSC
///*1*/	{720,288,50,	5,23,4 	3,2, 	1},	   	//PAL
///*2*/	{720,288,50,	5,23,4 	3,2, 	0},		//SECAM
///*3*/	{720,240,60,	9,21,0 	3,2, 	0},		//NTSC4.43
///*4*/	{720,240,60,	9,21,0 	3,2, 	0},		//PAL (M)
///*5*/	{720,288,50,	5,23,4 	3,2, 	0},		//PAL (CN)
///*6*/	{720,240,60,	9,21,0 	3,2, 	0}		//PAL 60
//};
#endif

//real value will be h-4,v-1.
//in digital FW will add h+4,v-1+2 on inputcrop.
//FW need F value on inputcrop
//        pol of PCLKO on output.
//and, if it possible, remove vDE offset.
code struct COMP_VIDEO_TIME_TABLE_s TW8836_COMP_TABLE[] = {							//pclok_pol, vDE_offset.
//                                                                             clamp   overscan pclko_pol 
/*002 480i   */ {7,	720,240,60,		I_HN_VN,	858, 16,104,	262, 2,16,		128,	3,2,  1},		//COMP_MODE_480I		
/*090 576i   */ {22,720,288,50,		I_HN_VN,	864, 16,117,	312, 2,20,		128,	3,2,  1},		//COMP_MODE_576I		
/*057 480p60 */ {2,	720,480,60,		P_HN_VN,	858, 16,107,	525, 2,34,		64,		3,2,  0},		//COMP_MODE_480P		
/*130 576p50 */ {18,720,576,50,		P_HN_VN,	864, 16,116,	625, 2,42,		58,		3,2,  0},		//COMP_MODE_576P
/*160 1080i25*/ {20,1920,540,50,	I_HP_VP,	2640,16,218,	563, 2,20,		40,		6,2,  0},		//COMP_MODE_1080I25		
/*164 1080i30*/ {5,	1920,540,60,	I_HP_VP,	2200,16,219,	562, 2,20,		32,		6,2,  1}, 		//COMP_MODE_1080I30		
/*150 720p50 */ {19,1280,720,50,	P_HP_VP,	1980,16,283,	750, 2,23,		38,		4,3,  1},		//COMP_MODE_720P50		
/*152 720p60 */ {4,	1280,720,60,	P_HP_VP,	1650,16,283,	750, 2,22,		38,		4,3,  0},		//COMP_MODE_720P60		
/*174 1080p50*/ {31,1920,1080,50,	P_HP_VP,	2640,16,218,	1125,2,39,		14,		6,3,  0},		//COMP_MODE_1080P50		
/*176 1080p60*/ {16,1920,1080,60,	P_HP_VP,	2200,16,218,	1125,2,39,		14,		6,3,  0},		//COMP_MODE_1080P60		
};
//1080@25 22 F:1 vDE:23
//1080@30 22 F:1 vDE:23


//VESA TABLE
//===========
//http://en.wikipedia.org/wiki/Graphics_display_resolution
//
//0x00 measn UNKNOWN value...Please let me know...
//If you add or remove item, you have to replace EEPROM. Use "ee format","ee init","ee default".
//BKFYI: EEPROM has 64 items for VESA table. Do not over 64 items.
code struct DIGIT_VIDEO_TIME_TABLE_s TW8836_VESA_TABLE[] = {									
/* 01   {  0,640,350,85,		P_HP_VN,	832,23,64,96,		445,32,3,60, 	0,31500 },*/
/* 02   {  0,640,400,85,		P_HN_VP,	832,32,64,96,		445,1,3,41,		0,31500 },*/
/* 03   {  0,720,400,85,		P_HN_VP,	936,36,72,108,		446,1,3,42,		0,35500 },*/
/* 04*/ {  1,640,480,60,		P_HN_VN,	800,8+8,96,40+8,	525,8+2,2,25+8, 0,25175},	//DMT0659 	#184:P_22 #187	 96,140,144 2,36
/* 05*/	{  1,640,480,72,		P_HN_VN,	832,8+16,40,120+8,	520,8+1,3,20+8, 0,31500 },  //DMT0672 	#189
/* 06*/	{  1,640,480,75,		P_HN_VN,	840,16,64,120,		500,1,3,16,		0,31500 },	//DMT0675	#190
/* 07*/	{  1,640,480,85,		P_HN_VN,	832,56,56,80,		509,1,3,25, 	0,36000 }, 	//DMT0685	#191
/* 08*/	{  1,800,600,56,		P_HP_VP,	1024,24,72,128,		625,1,2,22,		0,36000 },	//DMT0856	#195
/*>09*/	{  1,800,600,60,		P_HP_VP,	1056,40,128,88,		628,1,4,23,		9,40000 },	//DMT0860	#196	 NOTE:vDE scale offset
/*   */	{  1,800,600,70,		P_HP_VN,	1040,49,80,111,		625,2,3,20, 	0,00000 },	//...@PC
/* 0A*/	{  1,800,600,72,		P_HP_VP,	1040,56,120,64,		666,37,6,23,	0,50000},	//DMT0872	#197
/* 0B*/	{  1,800,600,75,		P_HP_VP,	1056,16,80,160,		625,1,3,21,		0,49500 },	//DMT0875	#198
/* 0C*/	{  1,800,600,85,		P_HP_VP, 	1048,32,64,152,		631,1,3,27,		0,56250 },	//DMT0885	#199
/* 0D   {  0,800,600,120,		P_HP_VN,	960,48,32,80,		636,3,4,29,		0,73250 }, */	//REDUCED Blanking
/* 0E*/	{  1,848,480,60,		P_HP_VP,	1088,16,112,112,	517,6,8,23,		0,33750 },
/* ??*/	{  1,960,600,60,		P_HN_VP,	1232,0x00,96,135,	622,0x00,4,23,	0,00000},	//CVT0860 ?
/*0Fh	{  0,1024,768,43,		I_HP_VP,	1264,8,176,56,		817,0,4,20,		0,44900 },*/
/*10h*/	{  1,1024,768,60,		P_HN_VN,	1344,24,136,160,	806,3,6,29,		0,65000 },	//DMT1060	#201
/*11h*/	{  1,1024,768,70,		P_HN_VN,	1328,24,136,144,	806,3,6,29,		0,75000 },	//DMT1070	#202
/*12h*/	{  1,1024,768,75,		P_HP_VP,	1312,16,96,176,		800,1,3,28,		0,78750 },	//DMT1075 	#203
/*13h*/	{  1,1024,768,85,		P_HP_VP,	1376,48,96,208,		808,1,3,36,		0,94500 },	//DMT1085	#204	new	
/*14h   {  0,1024,768,120,		P_HP_VN,	1184,48,32,80,		813,3,4,38,		0,115500 },*/
/*15h*/	{  1,1152,864,75,		P_HP_VP,	1600,64,128,256,	900,1,3,32,		0,108000 },	//DMT1175	#206
/*???	{  1,1280,720,60, 		P_HN_VP,	1650,110,125,220,	750,5,6,20,		0,00000}, */	//720P. #70+DSS	 #326  							 
/*16h*/	{  1,1280,768,60, 		P_HP_VN,	1440,48,32,80,		790,3,7,12,		0,68250},	  	//
/*17h*/	{  1,1280,768,60, 		P_HN_VP,	1664,64,128,192,	798,3,4,20,		0,79500},		//
/*??*/	{  1,1280,768,60, 		P_HN_VP,	1688,0x00,0x00,232,	802,0x00,0x00,34,	0, 00000},	
/*18h*/	{  1,1280,768,75, 		P_HN_VP,	1696,80,128,208,	805,3,7,27,		0,102250},		//
/*19h*/	{  1,1280,768,85, 		P_HN_VP,	1712,80,136,216,	809,3,7,31,		0,117500 },		//
/*1Ah   {  0,1280,768,120, 		P_HP_VN,	1440,48,32,80,		813,3,7,35,		0,140250 },*/									 
/*1Bh*/	{  1,1280,800,60, 		P_HP_VN,	1440,48,32,80,		823,3,6,14,     0,71000},	
/*1Ch*/	{  1,1280,800,60, 		P_HN_VP,	1680,72,128,200,	831,3,6,22,     0,83500},	
/*1Dh*/	{  1,1280,800,75, 		P_HN_VP,	1696,80,128,208,	838,3,6,29,     0,106500 },	
/*1Eh*/	{  1,1280,800,85, 		P_HN_VP,	1712,80,136,216,	843,3,6,34,		0,122500 },	
/*1Fh   {  0,1280,800,120, 		P_HP_VN,	1440,48,32,80,		847,3,6,38,		0,146250 },*/	
/*20h*/	{  1,1280,960,60, 		P_HP_VP,	1800,96,112,312,	1000,1,3,36,    0,108000 },		//DMT1260A 	#208
/*21h*/	{  1,1280,960,85, 		P_HP_VP,	1728,64,160,224,	1011,1,3,47,  	0,148500 },		//DMT1285A	#210 new		
/*22h   {  0,1280,960,120, 		P_HP_VN,	1440,48,32,80,		1017,3,4,50,	0,175500 }, */
/*23h*/	{  1,1280,1024,60, 		P_HP_VP,	1688,48,112,248,	1066,1,3,38,    0,108000 },		//DMT1260G	#212	
/*24h*/	{  1,1280,1024,75, 		P_HP_VP,	1688,16,144,248,	1066,1,3,38,	0,135000 },		//DMT1275G	#213 new		
/*25h*/	{  1,1280,1024,85, 		P_HP_VP,	1728,64,160,224,	1072,1,3,44,	0,157000	},		//DMT1285A	#214											 
/*26h	{  0,1280,1024,120,		P_HP_VN,	1440,48,32,80,		1084,3,7,50,	0,187250 },*/
/*27h*/	{  1,1360,768,60, 		P_HP_VP,	1792,64,112,256,	795,3,6,18,     0,85500},																  
/*28h	{  0,1360,768,120		P_HP_VN,	1520,48,32,80,		813,3,5,37,		0,148250}, */
/*29h*/	{  1,1400,1050,60, 		P_HP_VN,	1560,48,32,80,		1080,3,4,23,    0,101000 },		
/*2Ah*/	{  1,1400,1050,60, 		P_HN_VP,	1864,88,143,232,	1089,3,4,32,    0,121750 },		//measured #354	
/*2Bh	{  0,1400,1050,75,		P_HN_VP,	1896,104,144,248,	1099,3,4,42,	0,156000 },*/
/*2Ch	{  0,1400,1050,85,		P_HN_VP,	1912,104,152,256,	1105,3,4,48,	0,179500},*/
/*2Dh	{  0,1400,1050,120,		P_HP_VN,	1560,48,32,80,		1112,3,4,55,	0,208000}, */
/*2Eh*/	{  1,1440,900,60, 		P_HP_VN,	1600,48,32,80,		926,3,6,17,     0,88750	 },	//reduced blanking	
/*2Fh*/	{  1,1440,900,60, 		P_HN_VP,	1904,80,152,232,	934,3,6,25,     0,106500 },		
/*30h	{  0,1440,900,75, 		P_HN_VP,	1936,96,152,248,	942,3,6,33,		0,136750 },	*/
/*31h	{  0,1440,900,85		P_HN_VP,	1952,104,152,256,	948,3,6,39,		0,157000},  */
/*32h	{  0,1440,900,120,		P_HP_VN,	1600,48,32,80,		953,3,6,44,		0,182750},  */
/*33h	{  0,1600,1200,60,		P_HP_VP,	2160,64,192,304,	1250,1,3,46,	0,162000}, */
/*34h	{  0,1600,1200,65,		P_HP_VP,	2160,64,192,304,	1250,1,3,46,	0,175500}, */
/*35h	{  0,1600,1200,70,		P_HP_VP,	2160,64,192,304,	1250,1,3,46,	0,189000}, */
/*36h	{  0,1600,1200,75,		P_HP_VP,	2160,64,192,304,	1250,1,3,46,	0,202500}, */
/*37h	{  0,1600,1200,85,		P_HP_VP,	2160,64,192,304,	1250,1,3,46,	0,229500}, */
/*38h	{  0,1600,1200,120,		P_HP_VN,	1760,48,32,80,		1271,3,4,64,	0,268250}, */
/*39h*/	{  1,1680,1050,60,		P_HP_VN, 	1840,48,32,80,		1080,3,6,21,    0,119000	 },		// 1680x1440x60	REDUCED
/*3Ah	{  0,1680,1050,60,		P_HN_VP,	2240,104,176,280,	1089,3,6,30,	0,146250}, */
/*3Bh	{  0,1680,1050,75,		P_HN_VP,	2272,120,176,296,	1099,3,6,40,	0,187000}, */
/*3Ch	{  0,1680,1050,85,		P_HN_VP,	2288,128,176,304,	1105,3,6,46,	0,214750}, */
/*3Dh	{  0,1680,1050,120,		P_HP_VN,	1840,48,32,80,		1112,3,6,53,	0,245500}, */
/*3Eh	{  0,1792,1344,60,		P_HN_VP,	2448,128,200,328,	1394,1,3,46,	0,204750}, */
/*3Fh	{  0,1792,1344,75,		P_HN_VP,	2456,96,216,352,	1417,1,3,69,	0,261000}, */
/*40h	{  0,1792,1344,120,		P_HP_VN,	1952,48,32,80,		1423,3,4,72,	0,333250}, */
/*41h	{  0,1856,1392,60,		P_HN_VP,	2528,96,224,352,	1439,1,3,43,	0,218250}, */
/*42h	{  0,1856,1392,75,		P_HN_VP,	2560,128,224,352,	1500,1,3,104,	0,288000}, */
/*43h	{  0,1856,1392,120,		P_HP_VN,	2016,48,32,80,		1474,3,4,75,	0,356500}, */
/*44h	{  0,1920,1200,60,		P_HP_VN,	2080,48,32,80,		1235,3,6,26,	0,154000}, */
/*45h	{  0,1920,1200,60,		P_HN_VP,	2592,136,200,336,	1245,3,6,36,	0,193250}, */
/*46h	{  0,1920,1200,75,		P_HN_VP,	2608,136,208,344,	1255,3,6,46,	0,245250}, */
/*47h	{  0,1920,1200,85,		P_HN_VP,	2624,144,208,352,	1262,3,6,53,	0,281250}, */
/*48h	{  0,1920,1200,120,		P_HP_VN,	2080,48,32,80,		1271,3,6,62,	0,317000}, */
/*49h	{  0,1920,1440,60,		P_HN_VP,	2600,128,208,344,	1500,1,3,56,	0,234000}, */
/*4Ah	{  0,1920,1440,75,		P_HN_VP,	2640,144,224,352,	1500,1,3,56,	0,297000}, */
/*4Bh	{  0,1920,1440,120,		P_HP_VN,	2080,48,32,80,		1525,3,4,78,	0,380500}, */
/*4Ch	{  0,2560,1600,60,		P_HP_VN,	2720,48,32,80,		1646,3,6,37,	0,268500}, */
/*4Dh	{  0,2560,1600,60,		P_HN_VP,	3504,192,280,472,	1658,3,6,49,	0,348500}, */
/*4Eh	{  0,2560,1600,75,		P_HN_VP,	3536,208,280,488,	1672,3,6,63,	0,443250}, */
/*4Fh	{  0,2560,1600,85,		P_HN_VP,	3536,208,280,488,	1682,3,6,73,	0,505250}, */
/*50h	{  0,2560,1600,120,		P_HP_VN,	2720,48,32,80,		1694,3,6,85,	0,552750}, */
//add for component
		{  1,1440,288,50,		I_HN_VN,	1728, 24,126,138,  	312,2,3,19,  	0,27000}, //720x576I
		{  1,1440,240,60,		I_HN_VN,	1716, 38,124,114,  	262,4,3, 15,	0,27000}, //720x480I
		{  1,720,480,60,		P_HN_VN,	858,  16,62, 60, 	525,9,6, 30,	0,27000}, //720x480P			
		{  1,720,576,50,		P_HN_VN,	864,  12,64,68,  	625,5,5, 39, 	0,27000}, //720x576P
		{  1,1920,540,50,		I_HP_VP,	2640, 528,44,148,	566,2,5,15,   	0,74250L}, //1920x1080I@50
		{  1,1920,540,60,		I_HP_VP,	2200, 88,44, 148,  	562,2,5, 15,	0,47500L}, //1920x1080I@50
		{  1,1280,720,50,		P_HP_VP,	1980, 440,40,220,	750,5,5,20,		0,74250L}, //1280x720P@50
		{  1,1280,720,60,		P_HP_VP,	1650, 110,40,220, 	750,5,5, 20,	0,47500L}, //1280x720P@60
		{  1,1920,1080,50,		P_HN_VN,	2640, 528,44,148,	1125,4,5,36, 	0,148500L}, //1920x1080P@50  	
/*49*/	{  1,1920,1080,60,		P_HP_VP,	2200, 88,44,148, 	1124,4,5,36,    0,44000L}, //1920x1080P@60
//---------------------------------------
//--unknown. I don't have a source.
//---------------------------------------									 
/* ??   {  0,720,400,70,    	P_HN_VP,	900,18,108,53,		449,13,2,34, 	0,0 },*/	//DOS mode
/*			972,600,60						97,130,	223 ,227 	624,6, 22			*/	//#314 CVT0960D
/*END*/	{  0,	0,0,0, 			0x00,	0,0,0,0,			0,0,0,0}		// unknown
};
#undef O_X

code struct DIGIT_VIDEO_TIME_TABLE_s CEA861_table[] = {
/* 0 */	{1, 640,480,   60,P_HN_VN,	800,  16,96,48,		525,  10,2,33,	0,25175L},	/* #1		640x480p    @59.94*/
/* 1 */	{2, 720,480,   60,P_HN_VN,	858,  16,62,60, 	525,  3+6,6,30,	0,27000L},	/* #2,3		720x480p	@59.940 */			
/* 2 */	{4, 1280,720,  60,P_HP_VP,	1650, 110,40,220, 	750,  5,5,20,	0,44000L},	/* #4 		1280x720p	@59.939 pclk:74175000L */
/* 3*/	{5, 1920,540,  60,I_HP_VP,	2200, 88,44,148,  	562,  2,5,15,	0,47500L},	/* #5 		1920x1080i	@59.939 pclk:74175000L NOTE: "1920x540I"->"1080I"*/ 
/* 4*/	{6, 1440,240,  60,I_HN_VN,	1716, 38,124,114,  	262,  1+3,3,15,	0,27000L},  /* #6,7 	720(1440)x480i @59.940 */
/* 5*/	{8, 1440,240,  60,I_HN_VN,	1716, 38,124,114,	262,  1+3,3,15,	0,27000L},	/* #8,9 	720(1440)x240p @59.94 */
/* 6*/ 	{10, 2880,240, 60,P_HN_VN,	3432, 76,248,228,	262,  1+3,3,15,	0,54000L},	/* #10,11 	(2880)x480i @59.94 */
/* 7*/	{12, 2880,240, 60,P_HN_VN,	3432, 76,248,228,	262,  1+3,3,15,	0,54000L},	/* #12,13 	(2880)x240p@59.94 */
/* 8*/	{14, 1440,480, 60,I_HN_VN,	1716, 32,124,120,	525,  3+6,6,30,	0,54000L},	/* #14,15 	1440x480p@59.94 */
/* 9*/	{16, 1920,1080,60,P_HP_VP,	2200, 88,44,148,  	1125, 4,5,36,	0,44000L},	/* #16 		1920x1080p @59.939 pclk:148350000L */	   
/*10*/	{17, 720,576,  50,P_HN_VN,	864,  12,64,68,  	625,  5,5,39, 	0,27000L}, 	/* #17,18 	720x576p	@50Hz */
/*11*/	{19, 1280,720, 50,P_HP_VP,	1980, 440,40,220,	750,  5,5,20,	0,74250L},	/* #19 		1280x720p @50Hz  */
/*12*/	{20, 1920,540, 50,I_HP_VP,	2640, 528,44,148,	566,  2,5,15,  	0,74250L},	/* #20 		1920x1080i@50Hz */
/*13*/	{21, 1440,288, 50,P_HN_VN,	1728, 24,126,138,  	312,  2,3,19,  	0,27000L},  /* #21,22 	720(1440)x576i@50Hz */
/*14*/	{23, 1440,288, 50,P_HN_VN,	1728, 24,126,138,	312,  2,3,19,	0,27000L}, 	/* #23,24 	720(1440)x288p @50 */
/*15*/	{25, 2880,288, 50,P_HN_VN,	3456, 48,252,276,	312,  2,3,19, 	0,54000L},	/* #25,26 	(2880)x576i @50 */
/*16*/	{27, 2880,288, 50,P_HN_VN,	3456, 48,252,276,	312,  2,3,19, 	0,54000L},	/* #27,28 	(2880)x288p @25 */						
/*17*/	{29, 1440,576, 50,P_HN_VN,	1728, 24,128,136,  	625,  5,5,39,  	0,54000L}, 	/* #29,30 	1440x576p	@50Hz  */
/*18*/	{31, 1920,1080,50,P_HN_VN,	2640, 528,44,148,	1125, 4,5,36, 	0,148500L}, /* #31 		1920x1080p @50 */
/*19*/	{32, 1920,1080,24,I_HP_VP,	2750, 638,44,148,	1125, 4,5,36, 	0,74175L},	/* #32 		1920x1080p @23.97 */
/*20*/	{33, 1920,1080,25,I_HP_VP,	2640, 528,44,148,	1125, 4,5,36, 	0,74250L},	/* #33 		1920x1080p @25Hz */
/*21*/	{34, 1920,1080,30,I_HP_VP,	2640, 528,44,148,	1125, 4,5,36, 	0,74250L},	/* #34 		1920x1080p @29.97 */
//-----------------------------------
// convert table for 480i,576i,1080i
/*22 */	{35, 720,240,  60,I_HN_VN,	858,  19,62,57,  	262, 4,3, 15,	0,27000L },  		/* 480i */
/*23 */	{36, 720,288,  50,I_HN_VN,	864,  12,63,69,  	312, 2,3, 19,  	0,27000L },  		/* 576i */
/*24 */	{37, 1920,540, 60,I_HP_VP,	2200, 88,44, 148,  	652, 2,5, 15,	0,44000L },	 	/* 1080i pclk:74175000L */
/*22 	{35, 720,480,  60,I_HN_VN,	858,19,62,57,  		525, 3+6,6,30,	0,27000L },*/  	/* 480i */
/*23 	{36, 720,576,  50,I_HN_VN,	864,12,63,69,  		624, 4,6,38,	0,27000L },*/  	/* 576i */
/*24 	{37, 1920,1080,60,I_HP_VP,	2200, 88,44, 148,  	1124,4,10,30,	0,44000L },*/	 /* 1080i pclk:74175000L */
/*24 */	{37, 720,540, 60,I_HP_VP,	858, 19,62, 57,  	652, 2,5, 15,	0,30000L },	 	
/* for test */
/*25 */	{35,800,600,   60,P_HP_VP,	1056,40,128,88,		628, 1+0,4,23, 		0,40000L},     /* 800x600@60 */ 
/*26 	{36,800,480,   60,P_HP_VN,	1056,40,128,88,		525, 3+6,6,30,		0,33264L},  */    /* 800x480@60 */
//VESA
/*26 */	{36,800,480,   60,P_HP_VN,	960,48,32,80,		496, 3,7,6,			0,28500L},      /* 800x480@60*/
/*25 */	{35,800,400,   60,P_HP_VP,	1056,40,128,88,		419, 1,3,15, 		0,40000L},     /* 800x600@60 */ 

/*END*/	{0, 0xFFFF,0xFFFF}
};

