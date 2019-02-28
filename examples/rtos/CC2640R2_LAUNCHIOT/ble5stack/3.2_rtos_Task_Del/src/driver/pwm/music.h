#ifndef MUSIC_PWM_H
#define MUSIC_PWM_H

#ifdef __cplusplus
extern "C"
{
#endif
  
// 定义低音音名
#define L1 262
#define L2 294
#define L3 330
#define L4 349
#define L5 392
#define L6 440
#define L7 494

// 定义中音音名
#define M1 523
#define M2 587
#define M3 659
#define M4 698
#define M5 784
#define M6 880
#define M7 988

// 定义高音音名
#define H1 1047
#define H2 1175
#define H3 1319
#define H4 1397
#define H5 1568
#define H6 1760
#define H7 1976


#define MUSIC_LEN    17    
uint16_t music[MUSIC_LEN] =  {L6,L3,L5,L6,L6,M1,M1,M2,M1,L6,M1,M1,L5,M1,M2,M3,M5};

#ifdef __cplusplus
{
#endif // extern "C"

#endif // MUSIC_PWM_H