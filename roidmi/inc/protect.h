#ifndef __PROTECT_H_
#define __PROTECT_H_



void Input_vol_judge(void);
void Input_vol_judge_poron(void);
void Chk_Fuse(void);
void Chk_Fuse_poron(void);
void Chk_Cell_VolDIFF();
void Chk_Cell_VolDIFF_poron(void);
void Input_charger_detect_poron(void);
extern U8 xdata OIV ;
extern U8 xdata UIV ;
extern U8 xdata FOPEN ;
extern U8 xdata Odiff ;


void Input_charger_detect(void);

#endif









