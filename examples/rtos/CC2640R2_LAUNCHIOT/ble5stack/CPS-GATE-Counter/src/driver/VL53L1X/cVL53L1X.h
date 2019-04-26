#ifndef CVL53L1X_H
#define CVL53L1X_H

#define SOFT_RESET                                                                  0x0000
#define I2C_SLAVE__DEVICE_ADDRESS                                                   0x0001
#define ANA_CONFIG__VHV_REF_SEL_VDDPIX                                              0x0002
#define ANA_CONFIG__VHV_REF_SEL_VQUENCH                                             0x0003
#define ANA_CONFIG__REG_AVDD1V2_SEL                                                 0x0004
#define ANA_CONFIG__FAST_OSC__TRIM                                                  0x0005
#define OSC_MEASURED__FAST_OSC__FREQUENCY                                           0x0006
#define OSC_MEASURED__FAST_OSC__FREQUENCY_HI                                        0x0006
#define OSC_MEASURED__FAST_OSC__FREQUENCY_LO                                        0x0007
#define VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND                                       0x0008
#define VHV_CONFIG__COUNT_THRESH                                                    0x0009
#define VHV_CONFIG__OFFSET                                                          0x000A
#define VHV_CONFIG__INIT                                                            0x000B
#define GLOBAL_CONFIG__SPAD_ENABLES_REF_0                                           0x000D
#define GLOBAL_CONFIG__SPAD_ENABLES_REF_1                                           0x000E
#define GLOBAL_CONFIG__SPAD_ENABLES_REF_2                                           0x000F
#define GLOBAL_CONFIG__SPAD_ENABLES_REF_3                                           0x0010
#define GLOBAL_CONFIG__SPAD_ENABLES_REF_4                                           0x0011
#define GLOBAL_CONFIG__SPAD_ENABLES_REF_5                                           0x0012
#define GLOBAL_CONFIG__REF_EN_START_SELECT                                          0x0013
#define REF_SPAD_MAN__NUM_REQUESTED_REF_SPADS                                       0x0014
#define REF_SPAD_MAN__REF_LOCATION                                                  0x0015
#define ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS                              0x0016
#define ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS_HI                           0x0016
#define ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS_LO                           0x0017
#define ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS                          0x0018
#define ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS_HI                       0x0018
#define ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS_LO                       0x0019
#define ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS                          0x001A
#define ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS_HI                       0x001A
#define ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS_LO                       0x001B
#define REF_SPAD_CHAR__TOTAL_RATE_TARGET_MCPS                                       0x001C
#define REF_SPAD_CHAR__TOTAL_RATE_TARGET_MCPS_HI                                    0x001C
#define REF_SPAD_CHAR__TOTAL_RATE_TARGET_MCPS_LO                                    0x001D
#define ALGO__PART_TO_PART_RANGE_OFFSET_MM                                          0x001E
#define ALGO__PART_TO_PART_RANGE_OFFSET_MM_HI                                       0x001E
#define ALGO__PART_TO_PART_RANGE_OFFSET_MM_LO                                       0x001F
#define MM_CONFIG__INNER_OFFSET_MM                                                  0x0020
#define MM_CONFIG__INNER_OFFSET_MM_HI                                               0x0020
#define MM_CONFIG__INNER_OFFSET_MM_LO                                               0x0021
#define MM_CONFIG__OUTER_OFFSET_MM                                                  0x0022
#define MM_CONFIG__OUTER_OFFSET_MM_HI                                               0x0022
#define MM_CONFIG__OUTER_OFFSET_MM_LO                                               0x0023
#define DSS_CONFIG__TARGET_TOTAL_RATE_MCPS                                          0x0024
#define DSS_CONFIG__TARGET_TOTAL_RATE_MCPS_HI                                       0x0024
#define DSS_CONFIG__TARGET_TOTAL_RATE_MCPS_LO                                       0x0025
#define DEBUG__CTRL                                                                 0x0026
#define TEST_MODE__CTRL                                                             0x0027
#define CLK_GATING__CTRL                                                            0x0028
#define NVM_BIST__CTRL                                                              0x0029
#define NVM_BIST__NUM_NVM_WORDS                                                     0x002A
#define NVM_BIST__START_ADDRESS                                                     0x002B
#define HOST_IF__STATUS                                                             0x002C
#define PAD_I2C_HV__CONFIG                                                          0x002D
#define PAD_I2C_HV__EXTSUP_CONFIG                                                   0x002E
#define GPIO_HV_PAD__CTRL                                                           0x002F
#define GPIO_HV_MUX__CTRL                                                           0x0030
#define GPIO__TIO_HV_STATUS                                                         0x0031
#define GPIO__FIO_HV_STATUS                                                         0x0032
#define ANA_CONFIG__SPAD_SEL_PSWIDTH                                                0x0033
#define ANA_CONFIG__VCSEL_PULSE_WIDTH_OFFSET                                        0x0034
#define ANA_CONFIG__FAST_OSC__CONFIG_CTRL                                           0x0035
#define SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS                                   0x0036
#define SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS                                 0x0037
#define SIGMA_ESTIMATOR__SIGMA_REF_MM                                               0x0038
#define ALGO__CROSSTALK_COMPENSATION_VALID_HEIGHT_MM                                0x0039
#define SPARE_HOST_CONFIG__STATIC_CONFIG_SPARE_0                                    0x003A
#define SPARE_HOST_CONFIG__STATIC_CONFIG_SPARE_1                                    0x003B
#define ALGO__RANGE_IGNORE_THRESHOLD_MCPS                                           0x003C
#define ALGO__RANGE_IGNORE_THRESHOLD_MCPS_HI                                        0x003C
#define ALGO__RANGE_IGNORE_THRESHOLD_MCPS_LO                                        0x003D
#define ALGO__RANGE_IGNORE_VALID_HEIGHT_MM                                          0x003E
#define ALGO__RANGE_MIN_CLIP                                                        0x003F
#define ALGO__CONSISTENCY_CHECK__TOLERANCE                                          0x0040
#define SPARE_HOST_CONFIG__STATIC_CONFIG_SPARE_2                                    0x0041
#define SD_CONFIG__RESET_STAGES_MSB                                                 0x0042
#define SD_CONFIG__RESET_STAGES_LSB                                                 0x0043
#define GPH_CONFIG__STREAM_COUNT_UPDATE_VALUE                                       0x0044
#define GLOBAL_CONFIG__STREAM_DIVIDER                                               0x0045
#define SYSTEM__INTERRUPT_CONFIG_GPIO                                               0x0046
#define CAL_CONFIG__VCSEL_START                                                     0x0047
#define CAL_CONFIG__REPEAT_RATE                                                     0x0048
#define CAL_CONFIG__REPEAT_RATE_HI                                                  0x0048
#define CAL_CONFIG__REPEAT_RATE_LO                                                  0x0049
#define GLOBAL_CONFIG__VCSEL_WIDTH                                                  0x004A
#define PHASECAL_CONFIG__TIMEOUT_MACROP                                             0x004B
#define PHASECAL_CONFIG__TARGET                                                     0x004C
#define PHASECAL_CONFIG__OVERRIDE                                                   0x004D
#define DSS_CONFIG__ROI_MODE_CONTROL                                                0x004F
#define SYSTEM__THRESH_RATE_HIGH                                                    0x0050
#define SYSTEM__THRESH_RATE_HIGH_HI                                                 0x0050
#define SYSTEM__THRESH_RATE_HIGH_LO                                                 0x0051
#define SYSTEM__THRESH_RATE_LOW                                                     0x0052
#define SYSTEM__THRESH_RATE_LOW_HI                                                  0x0052
#define SYSTEM__THRESH_RATE_LOW_LO                                                  0x0053
#define DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT                                   0x0054
#define DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT_HI                                0x0054
#define DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT_LO                                0x0055
#define DSS_CONFIG__MANUAL_BLOCK_SELECT                                             0x0056
#define DSS_CONFIG__APERTURE_ATTENUATION                                            0x0057
#define DSS_CONFIG__MAX_SPADS_LIMIT                                                 0x0058
#define DSS_CONFIG__MIN_SPADS_LIMIT                                                 0x0059
#define MM_CONFIG__TIMEOUT_MACROP_A                                                 0x005A // added by Pololu for 16-bit accesses
#define MM_CONFIG__TIMEOUT_MACROP_A_HI                                              0x005A
#define MM_CONFIG__TIMEOUT_MACROP_A_LO                                              0x005B
#define MM_CONFIG__TIMEOUT_MACROP_B                                                 0x005C // added by Pololu for 16-bit accesses
#define MM_CONFIG__TIMEOUT_MACROP_B_HI                                              0x005C
#define MM_CONFIG__TIMEOUT_MACROP_B_LO                                              0x005D
#define RANGE_CONFIG__TIMEOUT_MACROP_A                                              0x005E // added by Pololu for 16-bit accesses
#define RANGE_CONFIG__TIMEOUT_MACROP_A_HI                                           0x005E
#define RANGE_CONFIG__TIMEOUT_MACROP_A_LO                                           0x005F
#define RANGE_CONFIG__VCSEL_PERIOD_A                                                0x0060
#define RANGE_CONFIG__TIMEOUT_MACROP_B                                              0x0061 // added by Pololu for 16-bit accesses
#define RANGE_CONFIG__TIMEOUT_MACROP_B_HI                                           0x0061
#define RANGE_CONFIG__TIMEOUT_MACROP_B_LO                                           0x0062
#define RANGE_CONFIG__VCSEL_PERIOD_B                                                0x0063
#define RANGE_CONFIG__SIGMA_THRESH                                                  0x0064
#define RANGE_CONFIG__SIGMA_THRESH_HI                                               0x0064
#define RANGE_CONFIG__SIGMA_THRESH_LO                                               0x0065
#define RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS                                 0x0066
#define RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS_HI                              0x0066
#define RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS_LO                              0x0067
#define RANGE_CONFIG__VALID_PHASE_LOW                                               0x0068
#define RANGE_CONFIG__VALID_PHASE_HIGH                                              0x0069
#define SYSTEM__INTERMEASUREMENT_PERIOD                                             0x006C
#define SYSTEM__INTERMEASUREMENT_PERIOD_3                                           0x006C
#define SYSTEM__INTERMEASUREMENT_PERIOD_2                                           0x006D
#define SYSTEM__INTERMEASUREMENT_PERIOD_1                                           0x006E
#define SYSTEM__INTERMEASUREMENT_PERIOD_0                                           0x006F
#define SYSTEM__FRACTIONAL_ENABLE                                                   0x0070
#define SYSTEM__GROUPED_PARAMETER_HOLD_0                                            0x0071
#define SYSTEM__THRESH_HIGH                                                         0x0072
#define SYSTEM__THRESH_HIGH_HI                                                      0x0072
#define SYSTEM__THRESH_HIGH_LO                                                      0x0073
#define SYSTEM__THRESH_LOW                                                          0x0074
#define SYSTEM__THRESH_LOW_HI                                                       0x0074
#define SYSTEM__THRESH_LOW_LO                                                       0x0075
#define SYSTEM__ENABLE_XTALK_PER_QUADRANT                                           0x0076
#define SYSTEM__SEED_CONFIG                                                         0x0077
#define SD_CONFIG__WOI_SD0                                                          0x0078
#define SD_CONFIG__WOI_SD1                                                          0x0079
#define SD_CONFIG__INITIAL_PHASE_SD0                                                0x007A
#define SD_CONFIG__INITIAL_PHASE_SD1                                                0x007B
#define SYSTEM__GROUPED_PARAMETER_HOLD_1                                            0x007C
#define SD_CONFIG__FIRST_ORDER_SELECT                                               0x007D
#define SD_CONFIG__QUANTIFIER                                                       0x007E
#define ROI_CONFIG__USER_ROI_CENTRE_SPAD                                            0x007F
#define ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE                               0x0080
#define SYSTEM__SEQUENCE_CONFIG                                                     0x0081
#define SYSTEM__GROUPED_PARAMETER_HOLD                                              0x0082
#define POWER_MANAGEMENT__GO1_POWER_FORCE                                           0x0083
#define SYSTEM__STREAM_COUNT_CTRL                                                   0x0084
#define FIRMWARE__ENABLE                                                            0x0085
#define SYSTEM__INTERRUPT_CLEAR                                                     0x0086
#define SYSTEM__MODE_START                                                          0x0087
#define RESULT__INTERRUPT_STATUS                                                    0x0088
#define RESULT__RANGE_STATUS                                                        0x0089
#define RESULT__REPORT_STATUS                                                       0x008A
#define RESULT__STREAM_COUNT                                                        0x008B
#define RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0                                      0x008C
#define RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0_HI                                   0x008C
#define RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0_LO                                   0x008D
#define RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0                                     0x008E
#define RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0_HI                                  0x008E
#define RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0_LO                                  0x008F
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD0                                         0x0090
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD0_HI                                      0x0090
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD0_LO                                      0x0091
#define RESULT__SIGMA_SD0                                                           0x0092
#define RESULT__SIGMA_SD0_HI                                                        0x0092
#define RESULT__SIGMA_SD0_LO                                                        0x0093
#define RESULT__PHASE_SD0                                                           0x0094
#define RESULT__PHASE_SD0_HI                                                        0x0094
#define RESULT__PHASE_SD0_LO                                                        0x0095
#define RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0                              0x0096
#define RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0_HI                           0x0096
#define RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0_LO                           0x0097
#define RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0                 0x0098
#define RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_HI              0x0098
#define RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_LO              0x0099
#define RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0                                 0x009A
#define RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0_HI                              0x009A
#define RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0_LO                              0x009B
#define RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0                                 0x009C
#define RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0_HI                              0x009C
#define RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0_LO                              0x009D
#define RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0                                      0x009E
#define RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0_HI                                   0x009E
#define RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0_LO                                   0x009F
#define RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1                                      0x00A0
#define RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1_HI                                   0x00A0
#define RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1_LO                                   0x00A1
#define RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1                                     0x00A2
#define RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1_HI                                  0x00A2
#define RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1_LO                                  0x00A3
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD1                                         0x00A4
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD1_HI                                      0x00A4
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD1_LO                                      0x00A5
#define RESULT__SIGMA_SD1                                                           0x00A6
#define RESULT__SIGMA_SD1_HI                                                        0x00A6
#define RESULT__SIGMA_SD1_LO                                                        0x00A7
#define RESULT__PHASE_SD1                                                           0x00A8
#define RESULT__PHASE_SD1_HI                                                        0x00A8
#define RESULT__PHASE_SD1_LO                                                        0x00A9
#define RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1                              0x00AA
#define RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1_HI                           0x00AA
#define RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1_LO                           0x00AB
#define RESULT__SPARE_0_SD1                                                         0x00AC
#define RESULT__SPARE_0_SD1_HI                                                      0x00AC
#define RESULT__SPARE_0_SD1_LO                                                      0x00AD
#define RESULT__SPARE_1_SD1                                                         0x00AE
#define RESULT__SPARE_1_SD1_HI                                                      0x00AE
#define RESULT__SPARE_1_SD1_LO                                                      0x00AF
#define RESULT__SPARE_2_SD1                                                         0x00B0
#define RESULT__SPARE_2_SD1_HI                                                      0x00B0
#define RESULT__SPARE_2_SD1_LO                                                      0x00B1
#define RESULT__SPARE_3_SD1                                                         0x00B2
#define RESULT__THRESH_INFO                                                         0x00B3
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0                                      0x00B4
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_3                                    0x00B4
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_2                                    0x00B5
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_1                                    0x00B6
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_0                                    0x00B7
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD0                                       0x00B8
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_3                                     0x00B8
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_2                                     0x00B9
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_1                                     0x00BA
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_0                                     0x00BB
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0                                        0x00BC
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_3                                      0x00BC
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_2                                      0x00BD
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_1                                      0x00BE
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_0                                      0x00BF
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0                                      0x00C0
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_3                                    0x00C0
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_2                                    0x00C1
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_1                                    0x00C2
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_0                                    0x00C3
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1                                      0x00C4
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_3                                    0x00C4
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_2                                    0x00C5
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_1                                    0x00C6
#define RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_0                                    0x00C7
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD1                                       0x00C8
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_3                                     0x00C8
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_2                                     0x00C9
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_1                                     0x00CA
#define RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_0                                     0x00CB
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1                                        0x00CC
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_3                                      0x00CC
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_2                                      0x00CD
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_1                                      0x00CE
#define RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_0                                      0x00CF
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1                                      0x00D0
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_3                                    0x00D0
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_2                                    0x00D1
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_1                                    0x00D2
#define RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_0                                    0x00D3
#define RESULT_CORE__SPARE_0                                                        0x00D4
#define PHASECAL_RESULT__REFERENCE_PHASE                                            0x00D6
#define PHASECAL_RESULT__REFERENCE_PHASE_HI                                         0x00D6
#define PHASECAL_RESULT__REFERENCE_PHASE_LO                                         0x00D7
#define PHASECAL_RESULT__VCSEL_START                                                0x00D8
#define REF_SPAD_CHAR_RESULT__NUM_ACTUAL_REF_SPADS                                  0x00D9
#define REF_SPAD_CHAR_RESULT__REF_LOCATION                                          0x00DA
#define VHV_RESULT__COLDBOOT_STATUS                                                 0x00DB
#define VHV_RESULT__SEARCH_RESULT                                                   0x00DC
#define VHV_RESULT__LATEST_SETTING                                                  0x00DD
#define RESULT__OSC_CALIBRATE_VAL                                                   0x00DE
#define RESULT__OSC_CALIBRATE_VAL_HI                                                0x00DE
#define RESULT__OSC_CALIBRATE_VAL_LO                                                0x00DF
#define ANA_CONFIG__POWERDOWN_GO1                                                   0x00E0
#define ANA_CONFIG__REF_BG_CTRL                                                     0x00E1
#define ANA_CONFIG__REGDVDD1V2_CTRL                                                 0x00E2
#define ANA_CONFIG__OSC_SLOW_CTRL                                                   0x00E3
#define TEST_MODE__STATUS                                                           0x00E4
#define FIRMWARE__SYSTEM_STATUS                                                     0x00E5
#define FIRMWARE__MODE_STATUS                                                       0x00E6
#define FIRMWARE__SECONDARY_MODE_STATUS                                             0x00E7
#define FIRMWARE__CAL_REPEAT_RATE_COUNTER                                           0x00E8
#define FIRMWARE__CAL_REPEAT_RATE_COUNTER_HI                                        0x00E8
#define FIRMWARE__CAL_REPEAT_RATE_COUNTER_LO                                        0x00E9
#define FIRMWARE__HISTOGRAM_BIN                                                     0x00EA
#define GPH__SYSTEM__THRESH_HIGH                                                    0x00EC
#define GPH__SYSTEM__THRESH_HIGH_HI                                                 0x00EC
#define GPH__SYSTEM__THRESH_HIGH_LO                                                 0x00ED
#define GPH__SYSTEM__THRESH_LOW                                                     0x00EE
#define GPH__SYSTEM__THRESH_LOW_HI                                                  0x00EE
#define GPH__SYSTEM__THRESH_LOW_LO                                                  0x00EF
#define GPH__SYSTEM__ENABLE_XTALK_PER_QUADRANT                                      0x00F0
#define GPH__SPARE_0                                                                0x00F1
#define GPH__SD_CONFIG__WOI_SD0                                                     0x00F2
#define GPH__SD_CONFIG__WOI_SD1                                                     0x00F3
#define GPH__SD_CONFIG__INITIAL_PHASE_SD0                                           0x00F4
#define GPH__SD_CONFIG__INITIAL_PHASE_SD1                                           0x00F5
#define GPH__SD_CONFIG__FIRST_ORDER_SELECT                                          0x00F6
#define GPH__SD_CONFIG__QUANTIFIER                                                  0x00F7
#define GPH__ROI_CONFIG__USER_ROI_CENTRE_SPAD                                       0x00F8
#define GPH__ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE                          0x00F9
#define GPH__SYSTEM__SEQUENCE_CONFIG                                                0x00FA
#define GPH__GPH_ID                                                                 0x00FB
#define SYSTEM__INTERRUPT_SET                                                       0x00FC
#define INTERRUPT_MANAGER__ENABLES                                                  0x00FD
#define INTERRUPT_MANAGER__CLEAR                                                    0x00FE
#define INTERRUPT_MANAGER__STATUS                                                   0x00FF
#define MCU_TO_HOST_BANK__WR_ACCESS_EN                                              0x0100
#define POWER_MANAGEMENT__GO1_RESET_STATUS                                          0x0101
#define PAD_STARTUP_MODE__VALUE_RO                                                  0x0102
#define PAD_STARTUP_MODE__VALUE_CTRL                                                0x0103
#define PLL_PERIOD_US                                                               0x0104
#define PLL_PERIOD_US_3                                                             0x0104
#define PLL_PERIOD_US_2                                                             0x0105
#define PLL_PERIOD_US_1                                                             0x0106
#define PLL_PERIOD_US_0                                                             0x0107
#define INTERRUPT_SCHEDULER__DATA_OUT                                               0x0108
#define INTERRUPT_SCHEDULER__DATA_OUT_3                                             0x0108
#define INTERRUPT_SCHEDULER__DATA_OUT_2                                             0x0109
#define INTERRUPT_SCHEDULER__DATA_OUT_1                                             0x010A
#define INTERRUPT_SCHEDULER__DATA_OUT_0                                             0x010B
#define NVM_BIST__COMPLETE                                                          0x010C
#define NVM_BIST__STATUS                                                            0x010D
#define IDENTIFICATION__MODEL_ID                                                    0x010F
#define IDENTIFICATION__MODULE_TYPE                                                 0x0110
#define IDENTIFICATION__REVISION_ID                                                 0x0111
#define IDENTIFICATION__MODULE_ID                                                   0x0112
#define IDENTIFICATION__MODULE_ID_HI                                                0x0112
#define IDENTIFICATION__MODULE_ID_LO                                                0x0113
#define ANA_CONFIG__FAST_OSC__TRIM_MAX                                              0x0114
#define ANA_CONFIG__FAST_OSC__FREQ_SET                                              0x0115
#define ANA_CONFIG__VCSEL_TRIM                                                      0x0116
#define ANA_CONFIG__VCSEL_SELION                                                    0x0117
#define ANA_CONFIG__VCSEL_SELION_MAX                                                0x0118
#define PROTECTED_LASER_SAFETY__LOCK_BIT                                            0x0119
#define LASER_SAFETY__KEY                                                           0x011A
#define LASER_SAFETY__KEY_RO                                                        0x011B
#define LASER_SAFETY__CLIP                                                          0x011C
#define LASER_SAFETY__MULT                                                          0x011D
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_0                                           0x011E
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_1                                           0x011F
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_2                                           0x0120
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_3                                           0x0121
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_4                                           0x0122
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_5                                           0x0123
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_6                                           0x0124
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_7                                           0x0125
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_8                                           0x0126
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_9                                           0x0127
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_10                                          0x0128
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_11                                          0x0129
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_12                                          0x012A
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_13                                          0x012B
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_14                                          0x012C
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_15                                          0x012D
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_16                                          0x012E
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_17                                          0x012F
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_18                                          0x0130
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_19                                          0x0131
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_20                                          0x0132
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_21                                          0x0133
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_22                                          0x0134
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_23                                          0x0135
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_24                                          0x0136
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_25                                          0x0137
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_26                                          0x0138
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_27                                          0x0139
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_28                                          0x013A
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_29                                          0x013B
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_30                                          0x013C
#define GLOBAL_CONFIG__SPAD_ENABLES_RTN_31                                          0x013D
#define ROI_CONFIG__MODE_ROI_CENTRE_SPAD                                            0x013E
#define ROI_CONFIG__MODE_ROI_XY_SIZE                                                0x013F
#define GO2_HOST_BANK_ACCESS__OVERRIDE                                              0x0300
#define MCU_UTIL_MULTIPLIER__MULTIPLICAND                                           0x0400
#define MCU_UTIL_MULTIPLIER__MULTIPLICAND_3                                         0x0400
#define MCU_UTIL_MULTIPLIER__MULTIPLICAND_2                                         0x0401
#define MCU_UTIL_MULTIPLIER__MULTIPLICAND_1                                         0x0402
#define MCU_UTIL_MULTIPLIER__MULTIPLICAND_0                                         0x0403
#define MCU_UTIL_MULTIPLIER__MULTIPLIER                                             0x0404
#define MCU_UTIL_MULTIPLIER__MULTIPLIER_3                                           0x0404
#define MCU_UTIL_MULTIPLIER__MULTIPLIER_2                                           0x0405
#define MCU_UTIL_MULTIPLIER__MULTIPLIER_1                                           0x0406
#define MCU_UTIL_MULTIPLIER__MULTIPLIER_0                                           0x0407
#define MCU_UTIL_MULTIPLIER__PRODUCT_HI                                             0x0408
#define MCU_UTIL_MULTIPLIER__PRODUCT_HI_3                                           0x0408
#define MCU_UTIL_MULTIPLIER__PRODUCT_HI_2                                           0x0409
#define MCU_UTIL_MULTIPLIER__PRODUCT_HI_1                                           0x040A
#define MCU_UTIL_MULTIPLIER__PRODUCT_HI_0                                           0x040B
#define MCU_UTIL_MULTIPLIER__PRODUCT_LO                                             0x040C
#define MCU_UTIL_MULTIPLIER__PRODUCT_LO_3                                           0x040C
#define MCU_UTIL_MULTIPLIER__PRODUCT_LO_2                                           0x040D
#define MCU_UTIL_MULTIPLIER__PRODUCT_LO_1                                           0x040E
#define MCU_UTIL_MULTIPLIER__PRODUCT_LO_0                                           0x040F
#define MCU_UTIL_MULTIPLIER__START                                                  0x0410
#define MCU_UTIL_MULTIPLIER__STATUS                                                 0x0411
#define MCU_UTIL_DIVIDER__START                                                     0x0412
#define MCU_UTIL_DIVIDER__STATUS                                                    0x0413
#define MCU_UTIL_DIVIDER__DIVIDEND                                                  0x0414
#define MCU_UTIL_DIVIDER__DIVIDEND_3                                                0x0414
#define MCU_UTIL_DIVIDER__DIVIDEND_2                                                0x0415
#define MCU_UTIL_DIVIDER__DIVIDEND_1                                                0x0416
#define MCU_UTIL_DIVIDER__DIVIDEND_0                                                0x0417
#define MCU_UTIL_DIVIDER__DIVISOR                                                   0x0418
#define MCU_UTIL_DIVIDER__DIVISOR_3                                                 0x0418
#define MCU_UTIL_DIVIDER__DIVISOR_2                                                 0x0419
#define MCU_UTIL_DIVIDER__DIVISOR_1                                                 0x041A
#define MCU_UTIL_DIVIDER__DIVISOR_0                                                 0x041B
#define MCU_UTIL_DIVIDER__QUOTIENT                                                  0x041C
#define MCU_UTIL_DIVIDER__QUOTIENT_3                                                0x041C
#define MCU_UTIL_DIVIDER__QUOTIENT_2                                                0x041D
#define MCU_UTIL_DIVIDER__QUOTIENT_1                                                0x041E
#define MCU_UTIL_DIVIDER__QUOTIENT_0                                                0x041F
#define TIMER0__VALUE_IN                                                            0x0420
#define TIMER0__VALUE_IN_3                                                          0x0420
#define TIMER0__VALUE_IN_2                                                          0x0421
#define TIMER0__VALUE_IN_1                                                          0x0422
#define TIMER0__VALUE_IN_0                                                          0x0423
#define TIMER1__VALUE_IN                                                            0x0424
#define TIMER1__VALUE_IN_3                                                          0x0424
#define TIMER1__VALUE_IN_2                                                          0x0425
#define TIMER1__VALUE_IN_1                                                          0x0426
#define TIMER1__VALUE_IN_0                                                          0x0427
#define TIMER0__CTRL                                                                0x0428
#define TIMER1__CTRL                                                                0x0429
#define MCU_GENERAL_PURPOSE__GP_0                                                   0x042C
#define MCU_GENERAL_PURPOSE__GP_1                                                   0x042D
#define MCU_GENERAL_PURPOSE__GP_2                                                   0x042E
#define MCU_GENERAL_PURPOSE__GP_3                                                   0x042F
#define MCU_RANGE_CALC__CONFIG                                                      0x0430
#define MCU_RANGE_CALC__OFFSET_CORRECTED_RANGE                                      0x0432
#define MCU_RANGE_CALC__OFFSET_CORRECTED_RANGE_HI                                   0x0432
#define MCU_RANGE_CALC__OFFSET_CORRECTED_RANGE_LO                                   0x0433
#define MCU_RANGE_CALC__SPARE_4                                                     0x0434
#define MCU_RANGE_CALC__SPARE_4_3                                                   0x0434
#define MCU_RANGE_CALC__SPARE_4_2                                                   0x0435
#define MCU_RANGE_CALC__SPARE_4_1                                                   0x0436
#define MCU_RANGE_CALC__SPARE_4_0                                                   0x0437
#define MCU_RANGE_CALC__AMBIENT_DURATION_PRE_CALC                                   0x0438
#define MCU_RANGE_CALC__AMBIENT_DURATION_PRE_CALC_HI                                0x0438
#define MCU_RANGE_CALC__AMBIENT_DURATION_PRE_CALC_LO                                0x0439
#define MCU_RANGE_CALC__ALGO_VCSEL_PERIOD                                           0x043C
#define MCU_RANGE_CALC__SPARE_5                                                     0x043D
#define MCU_RANGE_CALC__ALGO_TOTAL_PERIODS                                          0x043E
#define MCU_RANGE_CALC__ALGO_TOTAL_PERIODS_HI                                       0x043E
#define MCU_RANGE_CALC__ALGO_TOTAL_PERIODS_LO                                       0x043F
#define MCU_RANGE_CALC__ALGO_ACCUM_PHASE                                            0x0440
#define MCU_RANGE_CALC__ALGO_ACCUM_PHASE_3                                          0x0440
#define MCU_RANGE_CALC__ALGO_ACCUM_PHASE_2                                          0x0441
#define MCU_RANGE_CALC__ALGO_ACCUM_PHASE_1                                          0x0442
#define MCU_RANGE_CALC__ALGO_ACCUM_PHASE_0                                          0x0443
#define MCU_RANGE_CALC__ALGO_SIGNAL_EVENTS                                          0x0444
#define MCU_RANGE_CALC__ALGO_SIGNAL_EVENTS_3                                        0x0444
#define MCU_RANGE_CALC__ALGO_SIGNAL_EVENTS_2                                        0x0445
#define MCU_RANGE_CALC__ALGO_SIGNAL_EVENTS_1                                        0x0446
#define MCU_RANGE_CALC__ALGO_SIGNAL_EVENTS_0                                        0x0447
#define MCU_RANGE_CALC__ALGO_AMBIENT_EVENTS                                         0x0448
#define MCU_RANGE_CALC__ALGO_AMBIENT_EVENTS_3                                       0x0448
#define MCU_RANGE_CALC__ALGO_AMBIENT_EVENTS_2                                       0x0449
#define MCU_RANGE_CALC__ALGO_AMBIENT_EVENTS_1                                       0x044A
#define MCU_RANGE_CALC__ALGO_AMBIENT_EVENTS_0                                       0x044B
#define MCU_RANGE_CALC__SPARE_6                                                     0x044C
#define MCU_RANGE_CALC__SPARE_6_HI                                                  0x044C
#define MCU_RANGE_CALC__SPARE_6_LO                                                  0x044D
#define MCU_RANGE_CALC__ALGO_ADJUST_VCSEL_PERIOD                                    0x044E
#define MCU_RANGE_CALC__ALGO_ADJUST_VCSEL_PERIOD_HI                                 0x044E
#define MCU_RANGE_CALC__ALGO_ADJUST_VCSEL_PERIOD_LO                                 0x044F
#define MCU_RANGE_CALC__NUM_SPADS                                                   0x0450
#define MCU_RANGE_CALC__NUM_SPADS_HI                                                0x0450
#define MCU_RANGE_CALC__NUM_SPADS_LO                                                0x0451
#define MCU_RANGE_CALC__PHASE_OUTPUT                                                0x0452
#define MCU_RANGE_CALC__PHASE_OUTPUT_HI                                             0x0452
#define MCU_RANGE_CALC__PHASE_OUTPUT_LO                                             0x0453
#define MCU_RANGE_CALC__RATE_PER_SPAD_MCPS                                          0x0454
#define MCU_RANGE_CALC__RATE_PER_SPAD_MCPS_3                                        0x0454
#define MCU_RANGE_CALC__RATE_PER_SPAD_MCPS_2                                        0x0455
#define MCU_RANGE_CALC__RATE_PER_SPAD_MCPS_1                                        0x0456
#define MCU_RANGE_CALC__RATE_PER_SPAD_MCPS_0                                        0x0457
#define MCU_RANGE_CALC__SPARE_7                                                     0x0458
#define MCU_RANGE_CALC__SPARE_8                                                     0x0459
#define MCU_RANGE_CALC__PEAK_SIGNAL_RATE_MCPS                                       0x045A
#define MCU_RANGE_CALC__PEAK_SIGNAL_RATE_MCPS_HI                                    0x045A
#define MCU_RANGE_CALC__PEAK_SIGNAL_RATE_MCPS_LO                                    0x045B
#define MCU_RANGE_CALC__AVG_SIGNAL_RATE_MCPS                                        0x045C
#define MCU_RANGE_CALC__AVG_SIGNAL_RATE_MCPS_HI                                     0x045C
#define MCU_RANGE_CALC__AVG_SIGNAL_RATE_MCPS_LO                                     0x045D
#define MCU_RANGE_CALC__AMBIENT_RATE_MCPS                                           0x045E
#define MCU_RANGE_CALC__AMBIENT_RATE_MCPS_HI                                        0x045E
#define MCU_RANGE_CALC__AMBIENT_RATE_MCPS_LO                                        0x045F
#define MCU_RANGE_CALC__XTALK                                                       0x0460
#define MCU_RANGE_CALC__XTALK_HI                                                    0x0460
#define MCU_RANGE_CALC__XTALK_LO                                                    0x0461
#define MCU_RANGE_CALC__CALC_STATUS                                                 0x0462
#define MCU_RANGE_CALC__DEBUG                                                       0x0463
#define MCU_RANGE_CALC__PEAK_SIGNAL_RATE_XTALK_CORR_MCPS                            0x0464
#define MCU_RANGE_CALC__PEAK_SIGNAL_RATE_XTALK_CORR_MCPS_HI                         0x0464
#define MCU_RANGE_CALC__PEAK_SIGNAL_RATE_XTALK_CORR_MCPS_LO                         0x0465
#define MCU_RANGE_CALC__SPARE_0                                                     0x0468
#define MCU_RANGE_CALC__SPARE_1                                                     0x0469
#define MCU_RANGE_CALC__SPARE_2                                                     0x046A
#define MCU_RANGE_CALC__SPARE_3                                                     0x046B
#define PATCH__CTRL                                                                 0x0470
#define PATCH__JMP_ENABLES                                                          0x0472
#define PATCH__JMP_ENABLES_HI                                                       0x0472
#define PATCH__JMP_ENABLES_LO                                                       0x0473
#define PATCH__DATA_ENABLES                                                         0x0474
#define PATCH__DATA_ENABLES_HI                                                      0x0474
#define PATCH__DATA_ENABLES_LO                                                      0x0475
#define PATCH__OFFSET_0                                                             0x0476
#define PATCH__OFFSET_0_HI                                                          0x0476
#define PATCH__OFFSET_0_LO                                                          0x0477
#define PATCH__OFFSET_1                                                             0x0478
#define PATCH__OFFSET_1_HI                                                          0x0478
#define PATCH__OFFSET_1_LO                                                          0x0479
#define PATCH__OFFSET_2                                                             0x047A
#define PATCH__OFFSET_2_HI                                                          0x047A
#define PATCH__OFFSET_2_LO                                                          0x047B
#define PATCH__OFFSET_3                                                             0x047C
#define PATCH__OFFSET_3_HI                                                          0x047C
#define PATCH__OFFSET_3_LO                                                          0x047D
#define PATCH__OFFSET_4                                                             0x047E
#define PATCH__OFFSET_4_HI                                                          0x047E
#define PATCH__OFFSET_4_LO                                                          0x047F
#define PATCH__OFFSET_5                                                             0x0480
#define PATCH__OFFSET_5_HI                                                          0x0480
#define PATCH__OFFSET_5_LO                                                          0x0481
#define PATCH__OFFSET_6                                                             0x0482
#define PATCH__OFFSET_6_HI                                                          0x0482
#define PATCH__OFFSET_6_LO                                                          0x0483
#define PATCH__OFFSET_7                                                             0x0484
#define PATCH__OFFSET_7_HI                                                          0x0484
#define PATCH__OFFSET_7_LO                                                          0x0485
#define PATCH__OFFSET_8                                                             0x0486
#define PATCH__OFFSET_8_HI                                                          0x0486
#define PATCH__OFFSET_8_LO                                                          0x0487
#define PATCH__OFFSET_9                                                             0x0488
#define PATCH__OFFSET_9_HI                                                          0x0488
#define PATCH__OFFSET_9_LO                                                          0x0489
#define PATCH__OFFSET_10                                                            0x048A
#define PATCH__OFFSET_10_HI                                                         0x048A
#define PATCH__OFFSET_10_LO                                                         0x048B
#define PATCH__OFFSET_11                                                            0x048C
#define PATCH__OFFSET_11_HI                                                         0x048C
#define PATCH__OFFSET_11_LO                                                         0x048D
#define PATCH__OFFSET_12                                                            0x048E
#define PATCH__OFFSET_12_HI                                                         0x048E
#define PATCH__OFFSET_12_LO                                                         0x048F
#define PATCH__OFFSET_13                                                            0x0490
#define PATCH__OFFSET_13_HI                                                         0x0490
#define PATCH__OFFSET_13_LO                                                         0x0491
#define PATCH__OFFSET_14                                                            0x0492
#define PATCH__OFFSET_14_HI                                                         0x0492
#define PATCH__OFFSET_14_LO                                                         0x0493
#define PATCH__OFFSET_15                                                            0x0494
#define PATCH__OFFSET_15_HI                                                         0x0494
#define PATCH__OFFSET_15_LO                                                         0x0495
#define PATCH__ADDRESS_0                                                            0x0496
#define PATCH__ADDRESS_0_HI                                                         0x0496
#define PATCH__ADDRESS_0_LO                                                         0x0497
#define PATCH__ADDRESS_1                                                            0x0498
#define PATCH__ADDRESS_1_HI                                                         0x0498
#define PATCH__ADDRESS_1_LO                                                         0x0499
#define PATCH__ADDRESS_2                                                            0x049A
#define PATCH__ADDRESS_2_HI                                                         0x049A
#define PATCH__ADDRESS_2_LO                                                         0x049B
#define PATCH__ADDRESS_3                                                            0x049C
#define PATCH__ADDRESS_3_HI                                                         0x049C
#define PATCH__ADDRESS_3_LO                                                         0x049D
#define PATCH__ADDRESS_4                                                            0x049E
#define PATCH__ADDRESS_4_HI                                                         0x049E
#define PATCH__ADDRESS_4_LO                                                         0x049F
#define PATCH__ADDRESS_5                                                            0x04A0
#define PATCH__ADDRESS_5_HI                                                         0x04A0
#define PATCH__ADDRESS_5_LO                                                         0x04A1
#define PATCH__ADDRESS_6                                                            0x04A2
#define PATCH__ADDRESS_6_HI                                                         0x04A2
#define PATCH__ADDRESS_6_LO                                                         0x04A3
#define PATCH__ADDRESS_7                                                            0x04A4
#define PATCH__ADDRESS_7_HI                                                         0x04A4
#define PATCH__ADDRESS_7_LO                                                         0x04A5
#define PATCH__ADDRESS_8                                                            0x04A6
#define PATCH__ADDRESS_8_HI                                                         0x04A6
#define PATCH__ADDRESS_8_LO                                                         0x04A7
#define PATCH__ADDRESS_9                                                            0x04A8
#define PATCH__ADDRESS_9_HI                                                         0x04A8
#define PATCH__ADDRESS_9_LO                                                         0x04A9
#define PATCH__ADDRESS_10                                                           0x04AA
#define PATCH__ADDRESS_10_HI                                                        0x04AA
#define PATCH__ADDRESS_10_LO                                                        0x04AB
#define PATCH__ADDRESS_11                                                           0x04AC
#define PATCH__ADDRESS_11_HI                                                        0x04AC
#define PATCH__ADDRESS_11_LO                                                        0x04AD
#define PATCH__ADDRESS_12                                                           0x04AE
#define PATCH__ADDRESS_12_HI                                                        0x04AE
#define PATCH__ADDRESS_12_LO                                                        0x04AF
#define PATCH__ADDRESS_13                                                           0x04B0
#define PATCH__ADDRESS_13_HI                                                        0x04B0
#define PATCH__ADDRESS_13_LO                                                        0x04B1
#define PATCH__ADDRESS_14                                                           0x04B2
#define PATCH__ADDRESS_14_HI                                                        0x04B2
#define PATCH__ADDRESS_14_LO                                                        0x04B3
#define PATCH__ADDRESS_15                                                           0x04B4
#define PATCH__ADDRESS_15_HI                                                        0x04B4
#define PATCH__ADDRESS_15_LO                                                        0x04B5
#define SPI_ASYNC_MUX__CTRL                                                         0x04C0
#define CLK__CONFIG                                                                 0x04C4
#define GPIO_LV_MUX__CTRL                                                           0x04CC
#define GPIO_LV_PAD__CTRL                                                           0x04CD
#define PAD_I2C_LV__CONFIG                                                          0x04D0
#define PAD_STARTUP_MODE__VALUE_RO_GO1                                              0x04D4
#define HOST_IF__STATUS_GO1                                                         0x04D5
#define MCU_CLK_GATING__CTRL                                                        0x04D8
#define TEST__BIST_ROM_CTRL                                                         0x04E0
#define TEST__BIST_ROM_RESULT                                                       0x04E1
#define TEST__BIST_ROM_MCU_SIG                                                      0x04E2
#define TEST__BIST_ROM_MCU_SIG_HI                                                   0x04E2
#define TEST__BIST_ROM_MCU_SIG_LO                                                   0x04E3
#define TEST__BIST_RAM_CTRL                                                         0x04E4
#define TEST__BIST_RAM_RESULT                                                       0x04E5
#define TEST__TMC                                                                   0x04E8
#define TEST__PLL_BIST_MIN_THRESHOLD                                                0x04F0
#define TEST__PLL_BIST_MIN_THRESHOLD_HI                                             0x04F0
#define TEST__PLL_BIST_MIN_THRESHOLD_LO                                             0x04F1
#define TEST__PLL_BIST_MAX_THRESHOLD                                                0x04F2
#define TEST__PLL_BIST_MAX_THRESHOLD_HI                                             0x04F2
#define TEST__PLL_BIST_MAX_THRESHOLD_LO                                             0x04F3
#define TEST__PLL_BIST_COUNT_OUT                                                    0x04F4
#define TEST__PLL_BIST_COUNT_OUT_HI                                                 0x04F4
#define TEST__PLL_BIST_COUNT_OUT_LO                                                 0x04F5
#define TEST__PLL_BIST_GONOGO                                                       0x04F6
#define TEST__PLL_BIST_CTRL                                                         0x04F7
#define RANGING_CORE__DEVICE_ID                                                     0x0680
#define RANGING_CORE__REVISION_ID                                                   0x0681
#define RANGING_CORE__CLK_CTRL1                                                     0x0683
#define RANGING_CORE__CLK_CTRL2                                                     0x0684
#define RANGING_CORE__WOI_1                                                         0x0685
#define RANGING_CORE__WOI_REF_1                                                     0x0686
#define RANGING_CORE__START_RANGING                                                 0x0687
#define RANGING_CORE__LOW_LIMIT_1                                                   0x0690
#define RANGING_CORE__HIGH_LIMIT_1                                                  0x0691
#define RANGING_CORE__LOW_LIMIT_REF_1                                               0x0692
#define RANGING_CORE__HIGH_LIMIT_REF_1                                              0x0693
#define RANGING_CORE__QUANTIFIER_1_MSB                                              0x0694
#define RANGING_CORE__QUANTIFIER_1_LSB                                              0x0695
#define RANGING_CORE__QUANTIFIER_REF_1_MSB                                          0x0696
#define RANGING_CORE__QUANTIFIER_REF_1_LSB                                          0x0697
#define RANGING_CORE__AMBIENT_OFFSET_1_MSB                                          0x0698
#define RANGING_CORE__AMBIENT_OFFSET_1_LSB                                          0x0699
#define RANGING_CORE__AMBIENT_OFFSET_REF_1_MSB                                      0x069A
#define RANGING_CORE__AMBIENT_OFFSET_REF_1_LSB                                      0x069B
#define RANGING_CORE__FILTER_STRENGTH_1                                             0x069C
#define RANGING_CORE__FILTER_STRENGTH_REF_1                                         0x069D
#define RANGING_CORE__SIGNAL_EVENT_LIMIT_1_MSB                                      0x069E
#define RANGING_CORE__SIGNAL_EVENT_LIMIT_1_LSB                                      0x069F
#define RANGING_CORE__SIGNAL_EVENT_LIMIT_REF_1_MSB                                  0x06A0
#define RANGING_CORE__SIGNAL_EVENT_LIMIT_REF_1_LSB                                  0x06A1
#define RANGING_CORE__TIMEOUT_OVERALL_PERIODS_MSB                                   0x06A4
#define RANGING_CORE__TIMEOUT_OVERALL_PERIODS_LSB                                   0x06A5
#define RANGING_CORE__INVERT_HW                                                     0x06A6
#define RANGING_CORE__FORCE_HW                                                      0x06A7
#define RANGING_CORE__STATIC_HW_VALUE                                               0x06A8
#define RANGING_CORE__FORCE_CONTINUOUS_AMBIENT                                      0x06A9
#define RANGING_CORE__TEST_PHASE_SELECT_TO_FILTER                                   0x06AA
#define RANGING_CORE__TEST_PHASE_SELECT_TO_TIMING_GEN                               0x06AB
#define RANGING_CORE__INITIAL_PHASE_VALUE_1                                         0x06AC
#define RANGING_CORE__INITIAL_PHASE_VALUE_REF_1                                     0x06AD
#define RANGING_CORE__FORCE_UP_IN                                                   0x06AE
#define RANGING_CORE__FORCE_DN_IN                                                   0x06AF
#define RANGING_CORE__STATIC_UP_VALUE_1                                             0x06B0
#define RANGING_CORE__STATIC_UP_VALUE_REF_1                                         0x06B1
#define RANGING_CORE__STATIC_DN_VALUE_1                                             0x06B2
#define RANGING_CORE__STATIC_DN_VALUE_REF_1                                         0x06B3
#define RANGING_CORE__MONITOR_UP_DN                                                 0x06B4
#define RANGING_CORE__INVERT_UP_DN                                                  0x06B5
#define RANGING_CORE__CPUMP_1                                                       0x06B6
#define RANGING_CORE__CPUMP_2                                                       0x06B7
#define RANGING_CORE__CPUMP_3                                                       0x06B8
#define RANGING_CORE__OSC_1                                                         0x06B9
#define RANGING_CORE__PLL_1                                                         0x06BB
#define RANGING_CORE__PLL_2                                                         0x06BC
#define RANGING_CORE__REFERENCE_1                                                   0x06BD
#define RANGING_CORE__REFERENCE_3                                                   0x06BF
#define RANGING_CORE__REFERENCE_4                                                   0x06C0
#define RANGING_CORE__REFERENCE_5                                                   0x06C1
#define RANGING_CORE__REGAVDD1V2                                                    0x06C3
#define RANGING_CORE__CALIB_1                                                       0x06C4
#define RANGING_CORE__CALIB_2                                                       0x06C5
#define RANGING_CORE__CALIB_3                                                       0x06C6
#define RANGING_CORE__TST_MUX_SEL1                                                  0x06C9
#define RANGING_CORE__TST_MUX_SEL2                                                  0x06CA
#define RANGING_CORE__TST_MUX                                                       0x06CB
#define RANGING_CORE__GPIO_OUT_TESTMUX                                              0x06CC
#define RANGING_CORE__CUSTOM_FE                                                     0x06CD
#define RANGING_CORE__CUSTOM_FE_2                                                   0x06CE
#define RANGING_CORE__SPAD_READOUT                                                  0x06CF
#define RANGING_CORE__SPAD_READOUT_1                                                0x06D0
#define RANGING_CORE__SPAD_READOUT_2                                                0x06D1
#define RANGING_CORE__SPAD_PS                                                       0x06D2
#define RANGING_CORE__LASER_SAFETY_2                                                0x06D4
#define RANGING_CORE__NVM_CTRL__MODE                                                0x0780
#define RANGING_CORE__NVM_CTRL__PDN                                                 0x0781
#define RANGING_CORE__NVM_CTRL__PROGN                                               0x0782
#define RANGING_CORE__NVM_CTRL__READN                                               0x0783
#define RANGING_CORE__NVM_CTRL__PULSE_WIDTH_MSB                                     0x0784
#define RANGING_CORE__NVM_CTRL__PULSE_WIDTH_LSB                                     0x0785
#define RANGING_CORE__NVM_CTRL__HV_RISE_MSB                                         0x0786
#define RANGING_CORE__NVM_CTRL__HV_RISE_LSB                                         0x0787
#define RANGING_CORE__NVM_CTRL__HV_FALL_MSB                                         0x0788
#define RANGING_CORE__NVM_CTRL__HV_FALL_LSB                                         0x0789
#define RANGING_CORE__NVM_CTRL__TST                                                 0x078A
#define RANGING_CORE__NVM_CTRL__TESTREAD                                            0x078B
#define RANGING_CORE__NVM_CTRL__DATAIN_MMM                                          0x078C
#define RANGING_CORE__NVM_CTRL__DATAIN_LMM                                          0x078D
#define RANGING_CORE__NVM_CTRL__DATAIN_LLM                                          0x078E
#define RANGING_CORE__NVM_CTRL__DATAIN_LLL                                          0x078F
#define RANGING_CORE__NVM_CTRL__DATAOUT_MMM                                         0x0790
#define RANGING_CORE__NVM_CTRL__DATAOUT_LMM                                         0x0791
#define RANGING_CORE__NVM_CTRL__DATAOUT_LLM                                         0x0792
#define RANGING_CORE__NVM_CTRL__DATAOUT_LLL                                         0x0793
#define RANGING_CORE__NVM_CTRL__ADDR                                                0x0794
#define RANGING_CORE__NVM_CTRL__DATAOUT_ECC                                         0x0795
#define RANGING_CORE__RET_SPAD_EN_0                                                 0x0796
#define RANGING_CORE__RET_SPAD_EN_1                                                 0x0797
#define RANGING_CORE__RET_SPAD_EN_2                                                 0x0798
#define RANGING_CORE__RET_SPAD_EN_3                                                 0x0799
#define RANGING_CORE__RET_SPAD_EN_4                                                 0x079A
#define RANGING_CORE__RET_SPAD_EN_5                                                 0x079B
#define RANGING_CORE__RET_SPAD_EN_6                                                 0x079C
#define RANGING_CORE__RET_SPAD_EN_7                                                 0x079D
#define RANGING_CORE__RET_SPAD_EN_8                                                 0x079E
#define RANGING_CORE__RET_SPAD_EN_9                                                 0x079F
#define RANGING_CORE__RET_SPAD_EN_10                                                0x07A0
#define RANGING_CORE__RET_SPAD_EN_11                                                0x07A1
#define RANGING_CORE__RET_SPAD_EN_12                                                0x07A2
#define RANGING_CORE__RET_SPAD_EN_13                                                0x07A3
#define RANGING_CORE__RET_SPAD_EN_14                                                0x07A4
#define RANGING_CORE__RET_SPAD_EN_15                                                0x07A5
#define RANGING_CORE__RET_SPAD_EN_16                                                0x07A6
#define RANGING_CORE__RET_SPAD_EN_17                                                0x07A7
#define RANGING_CORE__SPAD_SHIFT_EN                                                 0x07BA
#define RANGING_CORE__SPAD_DISABLE_CTRL                                             0x07BB
#define RANGING_CORE__SPAD_EN_SHIFT_OUT_DEBUG                                       0x07BC
#define RANGING_CORE__SPI_MODE                                                      0x07BD
#define RANGING_CORE__GPIO_DIR                                                      0x07BE
#define RANGING_CORE__VCSEL_PERIOD                                                  0x0880
#define RANGING_CORE__VCSEL_START                                                   0x0881
#define RANGING_CORE__VCSEL_STOP                                                    0x0882
#define RANGING_CORE__VCSEL_1                                                       0x0885
#define RANGING_CORE__VCSEL_STATUS                                                  0x088D
#define RANGING_CORE__STATUS                                                        0x0980
#define RANGING_CORE__LASER_CONTINUITY_STATE                                        0x0981
#define RANGING_CORE__RANGE_1_MMM                                                   0x0982
#define RANGING_CORE__RANGE_1_LMM                                                   0x0983
#define RANGING_CORE__RANGE_1_LLM                                                   0x0984
#define RANGING_CORE__RANGE_1_LLL                                                   0x0985
#define RANGING_CORE__RANGE_REF_1_MMM                                               0x0986
#define RANGING_CORE__RANGE_REF_1_LMM                                               0x0987
#define RANGING_CORE__RANGE_REF_1_LLM                                               0x0988
#define RANGING_CORE__RANGE_REF_1_LLL                                               0x0989
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_1_MMM                                   0x098A
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_1_LMM                                   0x098B
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_1_LLM                                   0x098C
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_1_LLL                                   0x098D
#define RANGING_CORE__RANGING_TOTAL_EVENTS_1_MMM                                    0x098E
#define RANGING_CORE__RANGING_TOTAL_EVENTS_1_LMM                                    0x098F
#define RANGING_CORE__RANGING_TOTAL_EVENTS_1_LLM                                    0x0990
#define RANGING_CORE__RANGING_TOTAL_EVENTS_1_LLL                                    0x0991
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_1_MMM                                     0x0992
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_1_LMM                                     0x0993
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_1_LLM                                     0x0994
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_1_LLL                                     0x0995
#define RANGING_CORE__TOTAL_PERIODS_ELAPSED_1_MM                                    0x0996
#define RANGING_CORE__TOTAL_PERIODS_ELAPSED_1_LM                                    0x0997
#define RANGING_CORE__TOTAL_PERIODS_ELAPSED_1_LL                                    0x0998
#define RANGING_CORE__AMBIENT_MISMATCH_MM                                           0x0999
#define RANGING_CORE__AMBIENT_MISMATCH_LM                                           0x099A
#define RANGING_CORE__AMBIENT_MISMATCH_LL                                           0x099B
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_REF_1_MMM                               0x099C
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_REF_1_LMM                               0x099D
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_REF_1_LLM                               0x099E
#define RANGING_CORE__AMBIENT_WINDOW_EVENTS_REF_1_LLL                               0x099F
#define RANGING_CORE__RANGING_TOTAL_EVENTS_REF_1_MMM                                0x09A0
#define RANGING_CORE__RANGING_TOTAL_EVENTS_REF_1_LMM                                0x09A1
#define RANGING_CORE__RANGING_TOTAL_EVENTS_REF_1_LLM                                0x09A2
#define RANGING_CORE__RANGING_TOTAL_EVENTS_REF_1_LLL                                0x09A3
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_REF_1_MMM                                 0x09A4
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_REF_1_LMM                                 0x09A5
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_REF_1_LLM                                 0x09A6
#define RANGING_CORE__SIGNAL_TOTAL_EVENTS_REF_1_LLL                                 0x09A7
#define RANGING_CORE__TOTAL_PERIODS_ELAPSED_REF_1_MM                                0x09A8
#define RANGING_CORE__TOTAL_PERIODS_ELAPSED_REF_1_LM                                0x09A9
#define RANGING_CORE__TOTAL_PERIODS_ELAPSED_REF_1_LL                                0x09AA
#define RANGING_CORE__AMBIENT_MISMATCH_REF_MM                                       0x09AB
#define RANGING_CORE__AMBIENT_MISMATCH_REF_LM                                       0x09AC
#define RANGING_CORE__AMBIENT_MISMATCH_REF_LL                                       0x09AD
#define RANGING_CORE__GPIO_CONFIG__A0                                               0x0A00
#define RANGING_CORE__RESET_CONTROL__A0                                             0x0A01
#define RANGING_CORE__INTR_MANAGER__A0                                              0x0A02
#define RANGING_CORE__POWER_FSM_TIME_OSC__A0                                        0x0A06
#define RANGING_CORE__VCSEL_ATEST__A0                                               0x0A07
#define RANGING_CORE__VCSEL_PERIOD_CLIPPED__A0                                      0x0A08
#define RANGING_CORE__VCSEL_STOP_CLIPPED__A0                                        0x0A09
#define RANGING_CORE__CALIB_2__A0                                                   0x0A0A
#define RANGING_CORE__STOP_CONDITION__A0                                            0x0A0B
#define RANGING_CORE__STATUS_RESET__A0                                              0x0A0C
#define RANGING_CORE__READOUT_CFG__A0                                               0x0A0D
#define RANGING_CORE__WINDOW_SETTING__A0                                            0x0A0E
#define RANGING_CORE__VCSEL_DELAY__A0                                               0x0A1A
#define RANGING_CORE__REFERENCE_2__A0                                               0x0A1B
#define RANGING_CORE__REGAVDD1V2__A0                                                0x0A1D
#define RANGING_CORE__TST_MUX__A0                                                   0x0A1F
#define RANGING_CORE__CUSTOM_FE_2__A0                                               0x0A20
#define RANGING_CORE__SPAD_READOUT__A0                                              0x0A21
#define RANGING_CORE__CPUMP_1__A0                                                   0x0A22
#define RANGING_CORE__SPARE_REGISTER__A0                                            0x0A23
#define RANGING_CORE__VCSEL_CONT_STAGE5_BYPASS__A0                                  0x0A24
#define RANGING_CORE__RET_SPAD_EN_18                                                0x0A25
#define RANGING_CORE__RET_SPAD_EN_19                                                0x0A26
#define RANGING_CORE__RET_SPAD_EN_20                                                0x0A27
#define RANGING_CORE__RET_SPAD_EN_21                                                0x0A28
#define RANGING_CORE__RET_SPAD_EN_22                                                0x0A29
#define RANGING_CORE__RET_SPAD_EN_23                                                0x0A2A
#define RANGING_CORE__RET_SPAD_EN_24                                                0x0A2B
#define RANGING_CORE__RET_SPAD_EN_25                                                0x0A2C
#define RANGING_CORE__RET_SPAD_EN_26                                                0x0A2D
#define RANGING_CORE__RET_SPAD_EN_27                                                0x0A2E
#define RANGING_CORE__RET_SPAD_EN_28                                                0x0A2F
#define RANGING_CORE__RET_SPAD_EN_29                                                0x0A30
#define RANGING_CORE__RET_SPAD_EN_30                                                0x0A31
#define RANGING_CORE__RET_SPAD_EN_31                                                0x0A32
#define RANGING_CORE__REF_SPAD_EN_0__EWOK                                           0x0A33
#define RANGING_CORE__REF_SPAD_EN_1__EWOK                                           0x0A34
#define RANGING_CORE__REF_SPAD_EN_2__EWOK                                           0x0A35
#define RANGING_CORE__REF_SPAD_EN_3__EWOK                                           0x0A36
#define RANGING_CORE__REF_SPAD_EN_4__EWOK                                           0x0A37
#define RANGING_CORE__REF_SPAD_EN_5__EWOK                                           0x0A38
#define RANGING_CORE__REF_EN_START_SELECT                                           0x0A39
#define RANGING_CORE__REGDVDD1V2_ATEST__EWOK                                        0x0A41
#define SOFT_RESET_GO1                                                              0x0B00
#define PRIVATE__PATCH_BASE_ADDR_RSLV                                               0x0E00
#define PREV_SHADOW_RESULT__INTERRUPT_STATUS                                        0x0ED0
#define PREV_SHADOW_RESULT__RANGE_STATUS                                            0x0ED1
#define PREV_SHADOW_RESULT__REPORT_STATUS                                           0x0ED2
#define PREV_SHADOW_RESULT__STREAM_COUNT                                            0x0ED3
#define PREV_SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0                          0x0ED4
#define PREV_SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0_HI                       0x0ED4
#define PREV_SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0_LO                       0x0ED5
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0                         0x0ED6
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0_HI                      0x0ED6
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0_LO                      0x0ED7
#define PREV_SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD0                             0x0ED8
#define PREV_SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD0_HI                          0x0ED8
#define PREV_SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD0_LO                          0x0ED9
#define PREV_SHADOW_RESULT__SIGMA_SD0                                               0x0EDA
#define PREV_SHADOW_RESULT__SIGMA_SD0_HI                                            0x0EDA
#define PREV_SHADOW_RESULT__SIGMA_SD0_LO                                            0x0EDB
#define PREV_SHADOW_RESULT__PHASE_SD0                                               0x0EDC
#define PREV_SHADOW_RESULT__PHASE_SD0_HI                                            0x0EDC
#define PREV_SHADOW_RESULT__PHASE_SD0_LO                                            0x0EDD
#define PREV_SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0                  0x0EDE
#define PREV_SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0_HI               0x0EDE
#define PREV_SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0_LO               0x0EDF
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0     0x0EE0
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_HI  0x0EE0
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_LO  0x0EE1
#define PREV_SHADOW_RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0                     0x0EE2
#define PREV_SHADOW_RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0_HI                  0x0EE2
#define PREV_SHADOW_RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0_LO                  0x0EE3
#define PREV_SHADOW_RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0                     0x0EE4
#define PREV_SHADOW_RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0_HI                  0x0EE4
#define PREV_SHADOW_RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0_LO                  0x0EE5
#define PREV_SHADOW_RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0                          0x0EE6
#define PREV_SHADOW_RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0_HI                       0x0EE6
#define PREV_SHADOW_RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0_LO                       0x0EE7
#define PREV_SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1                          0x0EE8
#define PREV_SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1_HI                       0x0EE8
#define PREV_SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1_LO                       0x0EE9
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1                         0x0EEA
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1_HI                      0x0EEA
#define PREV_SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1_LO                      0x0EEB
#define PREV_SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD1                             0x0EEC
#define PREV_SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD1_HI                          0x0EEC
#define PREV_SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD1_LO                          0x0EED
#define PREV_SHADOW_RESULT__SIGMA_SD1                                               0x0EEE
#define PREV_SHADOW_RESULT__SIGMA_SD1_HI                                            0x0EEE
#define PREV_SHADOW_RESULT__SIGMA_SD1_LO                                            0x0EEF
#define PREV_SHADOW_RESULT__PHASE_SD1                                               0x0EF0
#define PREV_SHADOW_RESULT__PHASE_SD1_HI                                            0x0EF0
#define PREV_SHADOW_RESULT__PHASE_SD1_LO                                            0x0EF1
#define PREV_SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1                  0x0EF2
#define PREV_SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1_HI               0x0EF2
#define PREV_SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1_LO               0x0EF3
#define PREV_SHADOW_RESULT__SPARE_0_SD1                                             0x0EF4
#define PREV_SHADOW_RESULT__SPARE_0_SD1_HI                                          0x0EF4
#define PREV_SHADOW_RESULT__SPARE_0_SD1_LO                                          0x0EF5
#define PREV_SHADOW_RESULT__SPARE_1_SD1                                             0x0EF6
#define PREV_SHADOW_RESULT__SPARE_1_SD1_HI                                          0x0EF6
#define PREV_SHADOW_RESULT__SPARE_1_SD1_LO                                          0x0EF7
#define PREV_SHADOW_RESULT__SPARE_2_SD1                                             0x0EF8
#define PREV_SHADOW_RESULT__SPARE_2_SD1_HI                                          0x0EF8
#define PREV_SHADOW_RESULT__SPARE_2_SD1_LO                                          0x0EF9
#define PREV_SHADOW_RESULT__SPARE_3_SD1                                             0x0EFA
#define PREV_SHADOW_RESULT__SPARE_3_SD1_HI                                          0x0EFA
#define PREV_SHADOW_RESULT__SPARE_3_SD1_LO                                          0x0EFB
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0                          0x0EFC
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_3                        0x0EFC
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_2                        0x0EFD
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_1                        0x0EFE
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_0                        0x0EFF
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0                           0x0F00
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_3                         0x0F00
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_2                         0x0F01
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_1                         0x0F02
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_0                         0x0F03
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0                            0x0F04
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_3                          0x0F04
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_2                          0x0F05
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_1                          0x0F06
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_0                          0x0F07
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0                          0x0F08
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_3                        0x0F08
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_2                        0x0F09
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_1                        0x0F0A
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_0                        0x0F0B
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1                          0x0F0C
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_3                        0x0F0C
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_2                        0x0F0D
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_1                        0x0F0E
#define PREV_SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_0                        0x0F0F
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1                           0x0F10
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_3                         0x0F10
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_2                         0x0F11
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_1                         0x0F12
#define PREV_SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_0                         0x0F13
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1                            0x0F14
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_3                          0x0F14
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_2                          0x0F15
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_1                          0x0F16
#define PREV_SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_0                          0x0F17
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1                          0x0F18
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_3                        0x0F18
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_2                        0x0F19
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_1                        0x0F1A
#define PREV_SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_0                        0x0F1B
#define PREV_SHADOW_RESULT_CORE__SPARE_0                                            0x0F1C
#define RESULT__DEBUG_STATUS                                                        0x0F20
#define RESULT__DEBUG_STAGE                                                         0x0F21
#define GPH__SYSTEM__THRESH_RATE_HIGH                                               0x0F24
#define GPH__SYSTEM__THRESH_RATE_HIGH_HI                                            0x0F24
#define GPH__SYSTEM__THRESH_RATE_HIGH_LO                                            0x0F25
#define GPH__SYSTEM__THRESH_RATE_LOW                                                0x0F26
#define GPH__SYSTEM__THRESH_RATE_LOW_HI                                             0x0F26
#define GPH__SYSTEM__THRESH_RATE_LOW_LO                                             0x0F27
#define GPH__SYSTEM__INTERRUPT_CONFIG_GPIO                                          0x0F28
#define GPH__DSS_CONFIG__ROI_MODE_CONTROL                                           0x0F2F
#define GPH__DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT                              0x0F30
#define GPH__DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT_HI                           0x0F30
#define GPH__DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT_LO                           0x0F31
#define GPH__DSS_CONFIG__MANUAL_BLOCK_SELECT                                        0x0F32
#define GPH__DSS_CONFIG__MAX_SPADS_LIMIT                                            0x0F33
#define GPH__DSS_CONFIG__MIN_SPADS_LIMIT                                            0x0F34
#define GPH__MM_CONFIG__TIMEOUT_MACROP_A_HI                                         0x0F36
#define GPH__MM_CONFIG__TIMEOUT_MACROP_A_LO                                         0x0F37
#define GPH__MM_CONFIG__TIMEOUT_MACROP_B_HI                                         0x0F38
#define GPH__MM_CONFIG__TIMEOUT_MACROP_B_LO                                         0x0F39
#define GPH__RANGE_CONFIG__TIMEOUT_MACROP_A_HI                                      0x0F3A
#define GPH__RANGE_CONFIG__TIMEOUT_MACROP_A_LO                                      0x0F3B
#define GPH__RANGE_CONFIG__VCSEL_PERIOD_A                                           0x0F3C
#define GPH__RANGE_CONFIG__VCSEL_PERIOD_B                                           0x0F3D
#define GPH__RANGE_CONFIG__TIMEOUT_MACROP_B_HI                                      0x0F3E
#define GPH__RANGE_CONFIG__TIMEOUT_MACROP_B_LO                                      0x0F3F
#define GPH__RANGE_CONFIG__SIGMA_THRESH                                             0x0F40
#define GPH__RANGE_CONFIG__SIGMA_THRESH_HI                                          0x0F40
#define GPH__RANGE_CONFIG__SIGMA_THRESH_LO                                          0x0F41
#define GPH__RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS                            0x0F42
#define GPH__RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS_HI                         0x0F42
#define GPH__RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS_LO                         0x0F43
#define GPH__RANGE_CONFIG__VALID_PHASE_LOW                                          0x0F44
#define GPH__RANGE_CONFIG__VALID_PHASE_HIGH                                         0x0F45
#define FIRMWARE__INTERNAL_STREAM_COUNT_DIV                                         0x0F46
#define FIRMWARE__INTERNAL_STREAM_COUNTER_VAL                                       0x0F47
#define DSS_CALC__ROI_CTRL                                                          0x0F54
#define DSS_CALC__SPARE_1                                                           0x0F55
#define DSS_CALC__SPARE_2                                                           0x0F56
#define DSS_CALC__SPARE_3                                                           0x0F57
#define DSS_CALC__SPARE_4                                                           0x0F58
#define DSS_CALC__SPARE_5                                                           0x0F59
#define DSS_CALC__SPARE_6                                                           0x0F5A
#define DSS_CALC__SPARE_7                                                           0x0F5B
#define DSS_CALC__USER_ROI_SPAD_EN_0                                                0x0F5C
#define DSS_CALC__USER_ROI_SPAD_EN_1                                                0x0F5D
#define DSS_CALC__USER_ROI_SPAD_EN_2                                                0x0F5E
#define DSS_CALC__USER_ROI_SPAD_EN_3                                                0x0F5F
#define DSS_CALC__USER_ROI_SPAD_EN_4                                                0x0F60
#define DSS_CALC__USER_ROI_SPAD_EN_5                                                0x0F61
#define DSS_CALC__USER_ROI_SPAD_EN_6                                                0x0F62
#define DSS_CALC__USER_ROI_SPAD_EN_7                                                0x0F63
#define DSS_CALC__USER_ROI_SPAD_EN_8                                                0x0F64
#define DSS_CALC__USER_ROI_SPAD_EN_9                                                0x0F65
#define DSS_CALC__USER_ROI_SPAD_EN_10                                               0x0F66
#define DSS_CALC__USER_ROI_SPAD_EN_11                                               0x0F67
#define DSS_CALC__USER_ROI_SPAD_EN_12                                               0x0F68
#define DSS_CALC__USER_ROI_SPAD_EN_13                                               0x0F69
#define DSS_CALC__USER_ROI_SPAD_EN_14                                               0x0F6A
#define DSS_CALC__USER_ROI_SPAD_EN_15                                               0x0F6B
#define DSS_CALC__USER_ROI_SPAD_EN_16                                               0x0F6C
#define DSS_CALC__USER_ROI_SPAD_EN_17                                               0x0F6D
#define DSS_CALC__USER_ROI_SPAD_EN_18                                               0x0F6E
#define DSS_CALC__USER_ROI_SPAD_EN_19                                               0x0F6F
#define DSS_CALC__USER_ROI_SPAD_EN_20                                               0x0F70
#define DSS_CALC__USER_ROI_SPAD_EN_21                                               0x0F71
#define DSS_CALC__USER_ROI_SPAD_EN_22                                               0x0F72
#define DSS_CALC__USER_ROI_SPAD_EN_23                                               0x0F73
#define DSS_CALC__USER_ROI_SPAD_EN_24                                               0x0F74
#define DSS_CALC__USER_ROI_SPAD_EN_25                                               0x0F75
#define DSS_CALC__USER_ROI_SPAD_EN_26                                               0x0F76
#define DSS_CALC__USER_ROI_SPAD_EN_27                                               0x0F77
#define DSS_CALC__USER_ROI_SPAD_EN_28                                               0x0F78
#define DSS_CALC__USER_ROI_SPAD_EN_29                                               0x0F79
#define DSS_CALC__USER_ROI_SPAD_EN_30                                               0x0F7A
#define DSS_CALC__USER_ROI_SPAD_EN_31                                               0x0F7B
#define DSS_CALC__USER_ROI_0                                                        0x0F7C
#define DSS_CALC__USER_ROI_1                                                        0x0F7D
#define DSS_CALC__MODE_ROI_0                                                        0x0F7E
#define DSS_CALC__MODE_ROI_1                                                        0x0F7F
#define SIGMA_ESTIMATOR_CALC__SPARE_0                                               0x0F80
#define VHV_RESULT__PEAK_SIGNAL_RATE_MCPS                                           0x0F82
#define VHV_RESULT__PEAK_SIGNAL_RATE_MCPS_HI                                        0x0F82
#define VHV_RESULT__PEAK_SIGNAL_RATE_MCPS_LO                                        0x0F83
#define VHV_RESULT__SIGNAL_TOTAL_EVENTS_REF                                         0x0F84
#define VHV_RESULT__SIGNAL_TOTAL_EVENTS_REF_3                                       0x0F84
#define VHV_RESULT__SIGNAL_TOTAL_EVENTS_REF_2                                       0x0F85
#define VHV_RESULT__SIGNAL_TOTAL_EVENTS_REF_1                                       0x0F86
#define VHV_RESULT__SIGNAL_TOTAL_EVENTS_REF_0                                       0x0F87
#define PHASECAL_RESULT__PHASE_OUTPUT_REF                                           0x0F88
#define PHASECAL_RESULT__PHASE_OUTPUT_REF_HI                                        0x0F88
#define PHASECAL_RESULT__PHASE_OUTPUT_REF_LO                                        0x0F89
#define DSS_RESULT__TOTAL_RATE_PER_SPAD                                             0x0F8A
#define DSS_RESULT__TOTAL_RATE_PER_SPAD_HI                                          0x0F8A
#define DSS_RESULT__TOTAL_RATE_PER_SPAD_LO                                          0x0F8B
#define DSS_RESULT__ENABLED_BLOCKS                                                  0x0F8C
#define DSS_RESULT__NUM_REQUESTED_SPADS                                             0x0F8E
#define DSS_RESULT__NUM_REQUESTED_SPADS_HI                                          0x0F8E
#define DSS_RESULT__NUM_REQUESTED_SPADS_LO                                          0x0F8F
#define MM_RESULT__INNER_INTERSECTION_RATE                                          0x0F92
#define MM_RESULT__INNER_INTERSECTION_RATE_HI                                       0x0F92
#define MM_RESULT__INNER_INTERSECTION_RATE_LO                                       0x0F93
#define MM_RESULT__OUTER_COMPLEMENT_RATE                                            0x0F94
#define MM_RESULT__OUTER_COMPLEMENT_RATE_HI                                         0x0F94
#define MM_RESULT__OUTER_COMPLEMENT_RATE_LO                                         0x0F95
#define MM_RESULT__TOTAL_OFFSET                                                     0x0F96
#define MM_RESULT__TOTAL_OFFSET_HI                                                  0x0F96
#define MM_RESULT__TOTAL_OFFSET_LO                                                  0x0F97
#define XTALK_CALC__XTALK_FOR_ENABLED_SPADS                                         0x0F98
#define XTALK_CALC__XTALK_FOR_ENABLED_SPADS_3                                       0x0F98
#define XTALK_CALC__XTALK_FOR_ENABLED_SPADS_2                                       0x0F99
#define XTALK_CALC__XTALK_FOR_ENABLED_SPADS_1                                       0x0F9A
#define XTALK_CALC__XTALK_FOR_ENABLED_SPADS_0                                       0x0F9B
#define XTALK_RESULT__AVG_XTALK_USER_ROI_KCPS                                       0x0F9C
#define XTALK_RESULT__AVG_XTALK_USER_ROI_KCPS_3                                     0x0F9C
#define XTALK_RESULT__AVG_XTALK_USER_ROI_KCPS_2                                     0x0F9D
#define XTALK_RESULT__AVG_XTALK_USER_ROI_KCPS_1                                     0x0F9E
#define XTALK_RESULT__AVG_XTALK_USER_ROI_KCPS_0                                     0x0F9F
#define XTALK_RESULT__AVG_XTALK_MM_INNER_ROI_KCPS                                   0x0FA0
#define XTALK_RESULT__AVG_XTALK_MM_INNER_ROI_KCPS_3                                 0x0FA0
#define XTALK_RESULT__AVG_XTALK_MM_INNER_ROI_KCPS_2                                 0x0FA1
#define XTALK_RESULT__AVG_XTALK_MM_INNER_ROI_KCPS_1                                 0x0FA2
#define XTALK_RESULT__AVG_XTALK_MM_INNER_ROI_KCPS_0                                 0x0FA3
#define XTALK_RESULT__AVG_XTALK_MM_OUTER_ROI_KCPS                                   0x0FA4
#define XTALK_RESULT__AVG_XTALK_MM_OUTER_ROI_KCPS_3                                 0x0FA4
#define XTALK_RESULT__AVG_XTALK_MM_OUTER_ROI_KCPS_2                                 0x0FA5
#define XTALK_RESULT__AVG_XTALK_MM_OUTER_ROI_KCPS_1                                 0x0FA6
#define XTALK_RESULT__AVG_XTALK_MM_OUTER_ROI_KCPS_0                                 0x0FA7
#define RANGE_RESULT__ACCUM_PHASE                                                   0x0FA8
#define RANGE_RESULT__ACCUM_PHASE_3                                                 0x0FA8
#define RANGE_RESULT__ACCUM_PHASE_2                                                 0x0FA9
#define RANGE_RESULT__ACCUM_PHASE_1                                                 0x0FAA
#define RANGE_RESULT__ACCUM_PHASE_0                                                 0x0FAB
#define RANGE_RESULT__OFFSET_CORRECTED_RANGE                                        0x0FAC
#define RANGE_RESULT__OFFSET_CORRECTED_RANGE_HI                                     0x0FAC
#define RANGE_RESULT__OFFSET_CORRECTED_RANGE_LO                                     0x0FAD
#define SHADOW_PHASECAL_RESULT__VCSEL_START                                         0x0FAE
#define SHADOW_RESULT__INTERRUPT_STATUS                                             0x0FB0
#define SHADOW_RESULT__RANGE_STATUS                                                 0x0FB1
#define SHADOW_RESULT__REPORT_STATUS                                                0x0FB2
#define SHADOW_RESULT__STREAM_COUNT                                                 0x0FB3
#define SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0                               0x0FB4
#define SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0_HI                            0x0FB4
#define SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0_LO                            0x0FB5
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0                              0x0FB6
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0_HI                           0x0FB6
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD0_LO                           0x0FB7
#define SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD0                                  0x0FB8
#define SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD0_HI                               0x0FB8
#define SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD0_LO                               0x0FB9
#define SHADOW_RESULT__SIGMA_SD0                                                    0x0FBA
#define SHADOW_RESULT__SIGMA_SD0_HI                                                 0x0FBA
#define SHADOW_RESULT__SIGMA_SD0_LO                                                 0x0FBB
#define SHADOW_RESULT__PHASE_SD0                                                    0x0FBC
#define SHADOW_RESULT__PHASE_SD0_HI                                                 0x0FBC
#define SHADOW_RESULT__PHASE_SD0_LO                                                 0x0FBD
#define SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0                       0x0FBE
#define SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0_HI                    0x0FBE
#define SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0_LO                    0x0FBF
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0          0x0FC0
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_HI       0x0FC0
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_LO       0x0FC1
#define SHADOW_RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0                          0x0FC2
#define SHADOW_RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0_HI                       0x0FC2
#define SHADOW_RESULT__MM_INNER_ACTUAL_EFFECTIVE_SPADS_SD0_LO                       0x0FC3
#define SHADOW_RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0                          0x0FC4
#define SHADOW_RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0_HI                       0x0FC4
#define SHADOW_RESULT__MM_OUTER_ACTUAL_EFFECTIVE_SPADS_SD0_LO                       0x0FC5
#define SHADOW_RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0                               0x0FC6
#define SHADOW_RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0_HI                            0x0FC6
#define SHADOW_RESULT__AVG_SIGNAL_COUNT_RATE_MCPS_SD0_LO                            0x0FC7
#define SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1                               0x0FC8
#define SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1_HI                            0x0FC8
#define SHADOW_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD1_LO                            0x0FC9
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1                              0x0FCA
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1_HI                           0x0FCA
#define SHADOW_RESULT__PEAK_SIGNAL_COUNT_RATE_MCPS_SD1_LO                           0x0FCB
#define SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD1                                  0x0FCC
#define SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD1_HI                               0x0FCC
#define SHADOW_RESULT__AMBIENT_COUNT_RATE_MCPS_SD1_LO                               0x0FCD
#define SHADOW_RESULT__SIGMA_SD1                                                    0x0FCE
#define SHADOW_RESULT__SIGMA_SD1_HI                                                 0x0FCE
#define SHADOW_RESULT__SIGMA_SD1_LO                                                 0x0FCF
#define SHADOW_RESULT__PHASE_SD1                                                    0x0FD0
#define SHADOW_RESULT__PHASE_SD1_HI                                                 0x0FD0
#define SHADOW_RESULT__PHASE_SD1_LO                                                 0x0FD1
#define SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1                       0x0FD2
#define SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1_HI                    0x0FD2
#define SHADOW_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD1_LO                    0x0FD3
#define SHADOW_RESULT__SPARE_0_SD1                                                  0x0FD4
#define SHADOW_RESULT__SPARE_0_SD1_HI                                               0x0FD4
#define SHADOW_RESULT__SPARE_0_SD1_LO                                               0x0FD5
#define SHADOW_RESULT__SPARE_1_SD1                                                  0x0FD6
#define SHADOW_RESULT__SPARE_1_SD1_HI                                               0x0FD6
#define SHADOW_RESULT__SPARE_1_SD1_LO                                               0x0FD7
#define SHADOW_RESULT__SPARE_2_SD1                                                  0x0FD8
#define SHADOW_RESULT__SPARE_2_SD1_HI                                               0x0FD8
#define SHADOW_RESULT__SPARE_2_SD1_LO                                               0x0FD9
#define SHADOW_RESULT__SPARE_3_SD1                                                  0x0FDA
#define SHADOW_RESULT__THRESH_INFO                                                  0x0FDB
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0                               0x0FDC
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_3                             0x0FDC
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_2                             0x0FDD
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_1                             0x0FDE
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD0_0                             0x0FDF
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0                                0x0FE0
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_3                              0x0FE0
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_2                              0x0FE1
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_1                              0x0FE2
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD0_0                              0x0FE3
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0                                 0x0FE4
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_3                               0x0FE4
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_2                               0x0FE5
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_1                               0x0FE6
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD0_0                               0x0FE7
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0                               0x0FE8
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_3                             0x0FE8
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_2                             0x0FE9
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_1                             0x0FEA
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD0_0                             0x0FEB
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1                               0x0FEC
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_3                             0x0FEC
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_2                             0x0FED
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_1                             0x0FEE
#define SHADOW_RESULT_CORE__AMBIENT_WINDOW_EVENTS_SD1_0                             0x0FEF
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1                                0x0FF0
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_3                              0x0FF0
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_2                              0x0FF1
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_1                              0x0FF2
#define SHADOW_RESULT_CORE__RANGING_TOTAL_EVENTS_SD1_0                              0x0FF3
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1                                 0x0FF4
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_3                               0x0FF4
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_2                               0x0FF5
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_1                               0x0FF6
#define SHADOW_RESULT_CORE__SIGNAL_TOTAL_EVENTS_SD1_0                               0x0FF7
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1                               0x0FF8
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_3                             0x0FF8
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_2                             0x0FF9
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_1                             0x0FFA
#define SHADOW_RESULT_CORE__TOTAL_PERIODS_ELAPSED_SD1_0                             0x0FFB
#define SHADOW_RESULT_CORE__SPARE_0                                                 0x0FFC
#define SHADOW_PHASECAL_RESULT__REFERENCE_PHASE_HI                                  0x0FFE
#define SHADOW_PHASECAL_RESULT__REFERENCE_PHASE_LO                                  0x0FFF

#ifdef __cplusplus
extern "C"
{
#endif
  
#include <stdint.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <stdbool.h>

    typedef enum { VL_Short, VL_Medium, VL_Long, Unknown } DistanceMode;

    typedef enum{
      RangeValid                =   0,

      // "sigma estimator check is above the internal defined threshold"
      // (sigma = standard deviation of measurement)
      SigmaFail                 =   1,

      // "signal value is below the internal defined threshold"
      SignalFail                =   2,

      // "Target is below minimum detection threshold."
      RangeValidMinRangeClipped =   3,

      // "phase is out of bounds"
      // (nothing detected in range; try a longer distance mode if applicable)
      OutOfBoundsFail           =   4,

      // "HW or VCSEL failure"
      HardwareFail              =   5,

      // "The Range is valid but the wraparound check has not been done."
      RangeValidNoWrapCheckFail =   6,

      // "Wrapped target, not matching phases"
      // "no matching phase in other VCSEL period timing."
      WrapTargetFail            =   7,

      // "Internal algo underflow or overflow in lite ranging."
   // ProcessingFail            =   8: not used in API

      // "Specific to lite ranging."
      // should never occur with this lib (which uses low power auto ranging,
      // as the API does)
      XtalkSignalFail           =   9,

      // "1st interrupt when starting ranging in back to back mode. Ignore
      // data."
      // should never occur with this lib
      SynchronizationInt         =  10, // (the API spells this "syncronisation")

      // "All Range ok but object is result of multiple pulses merging together.
      // Used by RQL for merged pulse detection"
   // RangeValid MergedPulse    =  11: not used in API

      // "Used by RQL as different to phase fail."
   // TargetPresentLackOfSignal =  12:

      // "Target is below minimum detection threshold."
      MinRangeFail              =  13,

      // "The reported range is invalid"
   // RangeInvalid              =  14: can't actually be returned by API (range can never become negative, even after correction)

      // "No Update."
      None                      = 255,
    } RangeStatus;

    typedef struct{
      uint16_t range_mm;
      RangeStatus range_status;
      float peak_signal_count_rate_MCPS;
      float ambient_count_rate_MCPS;
    } RangingData;

    static RangingData ranging_data;
    
    // value used in measurement timing budget calculations
    // assumes PresetMode is LOWPOWER_AUTONOMOUS
    //
    // vhv = LOWPOWER_AUTO_VHV_LOOP_DURATION_US + LOWPOWERAUTO_VHV_LOOP_BOUND
    //       (tuning parm default) * LOWPOWER_AUTO_VHV_LOOP_DURATION_US
    //     = 245 + 3 * 245 = 980
    // TimingGuard = LOWPOWER_AUTO_OVERHEAD_BEFORE_A_RANGING +
    //               LOWPOWER_AUTO_OVERHEAD_BETWEEN_A_B_RANGING + vhv
    //             = 1448 + 2100 + 980 = 4528
    static const uint32_t TimingGuard = 4528;

    // value in DSS_CONFIG__TARGET_TOTAL_RATE_MCPS register, used in DSS
    // calculations
    static const uint16_t TargetRate = 0x0A00;

    // for storing values read from RESULT__RANGE_STATUS (0x0089)
    // through RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0_LOW
    // (0x0099)
    typedef struct {
      uint8_t range_status;
    // uint8_t report_status: not used
      uint8_t stream_count;
      uint16_t dss_actual_effective_spads_sd0;
   // uint16_t peak_signal_count_rate_mcps_sd0: not used
      uint16_t ambient_count_rate_mcps_sd0;
   // uint16_t sigma_sd0: not used
   // uint16_t phase_sd0: not used
      uint16_t final_crosstalk_corrected_range_mm_sd0;
      uint16_t peak_signal_count_rate_crosstalk_corrected_mcps_sd0;
    } ResultBuffer;

    // making this static would save RAM for multiple instances as long as there
    // aren't multiple sensors being read at the same time (e.g. on separate
    // I2C buses)
    static ResultBuffer results;

    static uint32_t io_timeout;
    static bool did_timeout;
    static uint32_t timeout_start_ms;

    static uint16_t fast_osc_frequency;
    static uint16_t osc_calibrate_val;

    static  bool calibrated;
    static uint8_t saved_vhv_init;
    static uint8_t saved_vhv_timeout;

    static DistanceMode distance_mode;

    static uint8_t last_status; // status of last I2C transmission
    
    void writeReg(uint16_t reg, uint8_t value);
    void writeReg16Bit(uint16_t reg, uint16_t value);
    void writeReg32Bit(uint16_t reg, uint32_t value);
    uint8_t readReg(uint16_t reg);
    uint16_t readReg16Bit(uint16_t reg);
    uint32_t readReg32Bit(uint16_t reg);

    void setAddress(uint8_t new_addr);
    static uint8_t getAddress() { return readReg(I2C_SLAVE__DEVICE_ADDRESS);}

    bool VL_init(bool io_2v8);

    bool setDistanceMode(DistanceMode mode);
    static DistanceMode getDistanceMode() { return distance_mode; }

    bool setMeasurementTimingBudget(uint32_t budget_us);
    uint32_t getMeasurementTimingBudget();

    void startContinuous(uint32_t period_ms);
    void stopContinuous();
    uint16_t read(bool blocking);
    static uint16_t readRangeContinuousMillimeters(bool blocking) { return read(blocking); } // alias of read()

    // check if sensor has new reading available
    // assumes interrupt is active low (GPIO_HV_MUX__CTRL bit 4 is 1)
    static bool dataReady() { return (readReg(GPIO__TIO_HV_STATUS) & 0x01) == 0; }

    static void setTimeout(uint16_t timeout) { io_timeout = timeout; }
    static uint16_t getTimeout() { return io_timeout; }
    bool timeoutOccurred();


    // Record the current time to check an upcoming timeout against
    static void startTimeout() { timeout_start_ms = (Clock_getTicks()*Clock_tickPeriod/1000); }

    // Check if timeout is enabled (set to nonzero value) and has expired
    static bool checkTimeoutExpired() {return (io_timeout > 0) && ((uint16_t)((Clock_getTicks()*Clock_tickPeriod/1000) - timeout_start_ms) > io_timeout); }

    void setupManualCalibration();
    void readResults();
    void updateDSS();
    void getRangingData();

    static uint32_t decodeTimeout(uint16_t reg_val);
    static uint16_t encodeTimeout(uint32_t timeout_mclks);
    static uint32_t timeoutMclksToMicroseconds(uint32_t timeout_mclks, uint32_t macro_period_us);
    static uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_us, uint32_t macro_period_us);
    uint32_t calcMacroPeriod(uint8_t vcsel_period);

    // Convert count rate from fixed point 9.7 format to float
    static float countRateFixedToFloat(uint16_t count_rate_fixed) { return (float)count_rate_fixed / (1 << 7); }
	
#ifdef __cplusplus
}
#endif
	
#endif