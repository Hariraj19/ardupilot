#include "Copter.h"

#if TOY_MODE_ENABLED == ENABLED

// times in 0.1s units
#define TOY_COMMAND_DELAY 15
#define TOY_LONG_PRESS_COUNT 15
#define TOY_LAND_MANUAL_DISARM_COUNT 40
#define TOY_LAND_DISARM_COUNT 1
#define TOY_LAND_ARM_COUNT 1
#define TOY_RIGHT_PRESS_COUNT 1
#define TOY_ACTION_DELAY_MS 200
#define TOY_DESCENT_SLOW_HEIGHT 5
#define TOY_DESCENT_SLOW_RAMP 3
#define TOY_DESCENT_SLOW_MIN 300
#define TOY_RESET_TURTLE_TIME 5000

#define ENABLE_LOAD_TEST 0

AP_Param*  ToyMode::Profile::param_ptr_to_acro_rp_p = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_acro_yaw_p = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_acro_balance_roll = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_acro_balance_pitch = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_acro_trainer = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_acro_rp_expo = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_acro_y_expo = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_acro_thr_mid = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_angle_max = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_accel_roll_max = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_accel_pitch_max = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_accel_yaw_max = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_pilot_speed_up = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_rc_feel_rp = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_loiter_speed_cms = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_loiter_jerk_max_cmsss = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_loiter_accel_cmss = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_loiter_accel_min_cmss = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_wp_speed_cms = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_wp_speed_up_cms = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_wp_speed_down_cms = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_wp_radius_cm = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_wp_accel_cms = nullptr;
AP_Param*  ToyMode::Profile::param_ptr_to_wp_accel_z_cms = nullptr;

const AP_Param::GroupInfo ToyMode::var_info[] = {

    // @Param: _ENABLE
    // @DisplayName: tmode enable 
    // @Description: tmode (or "toy" mode) gives a simplified user interface designed for mass market drones. Version1 is for the SkyViper V2450GPS. Version2 is for the F412 based boards
    // @Values: 0:Disabled,1:EnableVersion1,2:EnableVersion2
    // @User: Advanced
    AP_GROUPINFO_FLAGS("_ENABLE", 1, ToyMode, enable, 0, AP_PARAM_FLAG_ENABLE),

    // @Param: _MODE1
    // @DisplayName: Tmode first mode
    // @Description: This is the initial mode when the vehicle is first turned on.
    // @Values: 0:Stabilize,1:Acro,2:AltHold,3:Auto,4:Guided,5:Loiter,6:RTL,7:Circle,9:Land,11:Drift,13:Sport,14:Flip,15:AutoTune,16:PosHold,17:Brake,18:Throw,19:Avoid_ADSB,20:Guided_NoGPS,22:FlowHold
    // @User: Standard
    AP_GROUPINFO("_MODE1", 2, ToyMode, primary_mode[0], LOITER),

    // @Param: _MODE2
    // @DisplayName: Tmode second mode
    // @Description: This is the secondary mode.
    // @Values: 0:Stabilize,1:Acro,2:AltHold,3:Auto,4:Guided,5:Loiter,6:RTL,7:Circle,9:Land,11:Drift,13:Sport,14:Flip,15:AutoTune,16:PosHold,17:Brake,18:Throw,19:Avoid_ADSB,20:Guided_NoGPS,22:FlowHold
    // @User: Standard
    AP_GROUPINFO("_MODE2", 3, ToyMode, primary_mode[1], ALT_HOLD),

    // @Param: _ACTION1
    // @DisplayName: Tmode action 1
    // @Description: This is the action taken for the left action button
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_ACTION1", 4, ToyMode, actions[0], ACTION_TOGGLE_VIDEO),

    // @Param: _ACTION2
    // @DisplayName: Tmode action 2
    // @Description: This is the action taken for the right action button
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_ACTION2", 5, ToyMode, actions[1], ACTION_TAKE_PHOTO),

    // @Param: _ACTION3
    // @DisplayName: Tmode action 3
    // @Description: This is the action taken for the power button
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_ACTION3", 6, ToyMode, actions[2], ACTION_DISARM),

    // @Param: _ACTION4
    // @DisplayName: Tmode action 4
    // @Description: This is the action taken for the left action button while the mode button is pressed
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_ACTION4", 7, ToyMode, actions[3], ACTION_NONE),

    // @Param: _ACTION5
    // @DisplayName: Tmode action 5
    // @Description: This is the action taken for the right action button while the mode button is pressed
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_ACTION5", 8, ToyMode, actions[4], ACTION_NONE),

    // @Param: _ACTION6
    // @DisplayName: Tmode action 6
    // @Description: This is the action taken for the power button while the mode button is pressed
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_ACTION6", 9, ToyMode, actions[5], ACTION_NONE),

    // @Param: _MODE_BTN
    // @DisplayName: Tmode mode button action
    // @Description: This is the action taken for the mode button being pressed
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_MODE_BTN", 10, ToyMode, actions[6], ACTION_TOGGLE_MODE),

    // @Param: _MODE_LONG
    // @DisplayName: Tmode mode button long action
    // @Description: This is the action taken for a long press of the mode button
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest,24:ModeFlowHold
    // @User: Standard
    AP_GROUPINFO("_MODE_LONG", 11, ToyMode, actions[7], ACTION_NONE),

    // @Param: _TRIM_AUTO
    // @DisplayName: Stick auto trim limit
    // @Description: This is the amount of automatic stick trim that can be applied when disarmed with sticks not moving. It is a PWM limit value away from 1500
    // @Range: 0 100
    // @User: Standard
    AP_GROUPINFO("_TRIM_AUTO", 12, ToyMode, trim_auto, 50),

    // @Param: _LL_BTN
    // @DisplayName: Tmode launch/land button action
    // @Description: This is the action taken for the launch/land button being pressed
    // @Values: 0:None,1:TakePhoto,2:ToggleVideo,3:ModeAcro,4:ModeAltHold,5:ModeAuto,6:ModeLoiter,7:ModeRTL,8:ModeCircle,9:ModeLand,10:ModeDrift,11:ModeSport,12:ModeAutoTune,13:ModePosHold,14:ModeBrake,15:ModeThrow,16:Flip,17:ModeStabilize,18:Disarm,19:ToggleMode,20:Arm-Land-RTL,21:ToggleSimpleMode,22:ToggleSuperSimpleMode,23:MotorLoadTest
    // @User: Standard
    AP_GROUPINFO("_LL_BTN", 13, ToyMode, actions[8], ACTION_ARM_LAND_RTL),

    // @Param: _FLAGS
    // @DisplayName: Tmode flags
    // @Description: Bitmask of flags to change the behaviour of tmode. DisarmOnLowThrottle means to disarm if throttle is held down for 1 second when landed. ArmOnHighThrottle means to arm if throttle is above 80% for 1 second. UpgradeToLoiter means to allow takeoff in LOITER mode by switching to ALT_HOLD, then auto-upgrading to LOITER once GPS is available. RTLStickCancel means that on large stick inputs in RTL mode that LOITER mode is engaged
    // @Bitmask: 0:DisarmOnLowThrottle,1:ArmOnHighThrottle,2:UpgradeToLoiter,3:RTLStickCancel
    // @User: Standard
    AP_GROUPINFO("_FLAGS", 14, ToyMode, flags, FLAG_THR_DISARM),

    // @Param: _VMIN
    // @DisplayName: Min voltage for output limiting
    // @Description: This is the battery voltage below which no output limiting is done
    // @Range: 0 5
    // @Increment: 0.01
    // @User: Advanced
    AP_GROUPINFO("_VMIN", 15, ToyMode, filter.volt_min, 3.5),

    // @Param: _VMAX
    // @DisplayName: Max voltage for output limiting
    // @Description: This is the battery voltage above which thrust min is used
    // @Range: 0 5
    // @Increment: 0.01
    // @User: Advanced
    AP_GROUPINFO("_VMAX", 16, ToyMode, filter.volt_max, 3.8),
    
    // @Param: _TMIN
    // @DisplayName: Min thrust multiplier
    // @Description: This sets the thrust multiplier when voltage is high
    // @Range: 0 1
    // @Increment: 0.01
    // @User: Advanced
    AP_GROUPINFO("_TMIN", 17, ToyMode, filter.thrust_min, 1.0),

    // @Param: _TMAX
    // @DisplayName: Max thrust multiplier
    // @Description: This sets the thrust multiplier when voltage is low
    // @Range: 0 1
    // @Increment: 0.01
    // @User: Advanced
    AP_GROUPINFO("_TMAX", 18, ToyMode, filter.thrust_max, 1.0),

    // @Param: _PROFILE_ID
    // @DisplayName: Profile ID
    // @Description: This sets the activated profile, 0 means no profile activated
    // @Range: 0 4
    // @User: Advanced
    AP_GROUPINFO("_PROFILE_ID", 19, ToyMode, profile_id, 0),

    // @Group: _P1
    // @Description: Profile set 1
    AP_SUBGROUPINFO(_var_info_profile[0], "_P1_", 20, ToyMode, ToyMode::Profile),

    // @Group: _P2
    // @Description: Profile set 2
    AP_SUBGROUPINFO(_var_info_profile[1], "_P2_", 21, ToyMode, ToyMode::Profile),

    // @Group: _P3
    // @Description: Profile set 3
    AP_SUBGROUPINFO(_var_info_profile[2], "_P3_", 22, ToyMode, ToyMode::Profile),

    // @Group: _P4
    // @Description: Profile set 4
    AP_SUBGROUPINFO(_var_info_profile[3], "_P4_", 23, ToyMode, ToyMode::Profile),

#if ENABLE_LOAD_TEST
    // @Param: _LOAD_MUL
    // @DisplayName: Load test multiplier
    // @Description: This scales the load test output, as a value between 0 and 1
    // @Range: 0 1
    // @Increment: 0.01
    // @User: Advanced
    AP_GROUPINFO("_LOAD_MUL", 24, ToyMode, load_test.load_mul, 1.0),
    
    // @Param: _LOAD_FILT
    // @DisplayName: Load test filter
    // @Description: This filters the load test output. A value of 1 means no filter. 2 means values are repeated once. 3 means values are repeated 3 times, etc
    // @Range: 0 100
    // @User: Advanced
    AP_GROUPINFO("_LOAD_FILT", 25, ToyMode, load_test.load_filter, 1),
    
    // @Param: _LOAD_TYPE
    // @DisplayName: Load test type
    // @Description: This sets the type of load test
    // @Values: 0:ConstantThrust,1:LogReplay1,2:LogReplay2
    // @User: Advanced
    AP_GROUPINFO("_LOAD_TYPE", 26, ToyMode, load_test.load_type, LOAD_TYPE_LOG1),
#endif

    AP_GROUPEND
};

const AP_Param::GroupInfo ToyMode::Profile::var_info[] = {

    // @Param: A_RP_P
    // @DisplayName: Acro Roll and Pitch P gain
    // @Description: Converts pilot roll and pitch into a desired rate of rotation in ACRO and SPORT mode.  Higher values mean faster rate of rotation.
    // @Range: 1 10
    // @User: Standard
    AP_GROUPINFO("A_RP_P", 1, ToyMode::Profile, acro_rp_p, ACRO_RP_P),

    // @Param: A_YAW_P
    // @DisplayName: Acro Yaw P gain
    // @Description: Converts pilot yaw input into a desired rate of rotation in ACRO, Stabilize and SPORT modes.  Higher values mean faster rate of rotation.
    // @Range: 1 10
    // @User: Standard
    AP_GROUPINFO("A_YAW_P", 2, ToyMode::Profile, acro_yaw_p, ACRO_YAW_P),

    // @Param: A_BAL_RL
    // @DisplayName: Acro Balance Roll
    // @Description: rate at which roll angle returns to level in acro mode.  A higher value causes the vehicle to return to level faster.
    // @Range: 0 3
    // @Increment: 0.1
    // @User: Advanced
    AP_GROUPINFO("A_BAL_R", 3, ToyMode::Profile, acro_balance_roll, ACRO_BALANCE_ROLL),

    // @Param: A_BAL_PI
    // @DisplayName: Acro Balance Pitch
    // @Description: rate at which pitch angle returns to level in acro mode.  A higher value causes the vehicle to return to level faster.
    // @Range: 0 3
    // @Increment: 0.1
    // @User: Advanced
    AP_GROUPINFO("A_BAL_P", 4, ToyMode::Profile, acro_balance_pitch, ACRO_BALANCE_PITCH),

    // @Param: A_TRNR
    // @DisplayName: Acro Trainer
    // @Description: Type of trainer used in acro mode
    // @Values: 0:Disabled,1:Leveling,2:Leveling and Limited
    // @User: Advanced
    AP_GROUPINFO("A_TRNR", 5, ToyMode::Profile, acro_trainer, ACRO_TRAINER_LIMITED),

    // @Param: A_RP_XPO
    // @DisplayName: Acro Roll/Pitch Expo
    // @Description: Acro roll/pitch Expo to allow faster rotation when stick at edges
    // @Values: 0:Disabled,0.1:Very Low,0.2:Low,0.3:Medium,0.4:High,0.5:Very High
    // @Range: -0.5 1.0
    // @User: Advanced
    AP_GROUPINFO("A_RP_XP", 6, ToyMode::Profile, acro_rp_expo, ACRO_RP_EXPO_DEFAULT),

    // @Param: A_Y_XPO
    // @DisplayName: Acro Yaw Expo
    // @Description: Acro yaw expo to allow faster rotation when stick at edges
    // @Values: 0:Disabled,0.1:Very Low,0.2:Low,0.3:Medium,0.4:High,0.5:Very High
    // @Range: -0.5 1.0
    // @User: Advanced
    AP_GROUPINFO("A_Y_XP", 7, ToyMode::Profile, acro_y_expo, ACRO_Y_EXPO_DEFAULT),

    // @Param: A_THR_M
    // @DisplayName: Acro Thr Mid
    // @Description: Acro Throttle Mid
    // @Range: 0 1
    // @User: Advanced
    AP_GROUPINFO("A_THR_M", 8, ToyMode::Profile, acro_thr_mid, ACRO_THR_MID_DEFAULT),

    // @Param: ANG_MAX
    // @DisplayName: Angle Max
    // @Description: Maximum lean angle in all flight modes
    // @Units: cdeg
    // @Range: 1000 8000
    // @User: Advanced
    AP_GROUPINFO("ANG_MAX", 9, ToyMode::Profile, angle_max, DEFAULT_ANGLE_MAX),

    // @Param: XL_Y_MX
    // @DisplayName: Acceleration Max for Yaw
    // @Description: Maximum acceleration in yaw axis
    // @Units: cdeg/s/s
    // @Range: 0 72000
    // @Values: 0:Disabled, 9000:VerySlow, 18000:Slow, 36000:Medium, 54000:Fast
    // @Increment: 1000
    // @User: Advanced
    AP_GROUPINFO("XL_Y_MX",  10, ToyMode::Profile, accel_yaw_max, AC_ATTITUDE_CONTROL_ACCEL_Y_MAX_DEFAULT_CDSS),

    // @Param: XL_R_MX
    // @DisplayName: Acceleration Max for Roll
    // @Description: Maximum acceleration in roll axis
    // @Units: cdeg/s/s
    // @Range: 0 180000
    // @Increment: 1000
    // @Values: 0:Disabled, 30000:VerySlow, 72000:Slow, 108000:Medium, 162000:Fast
    // @User: Advanced
    AP_GROUPINFO("XL_R_MX", 11, ToyMode::Profile, accel_roll_max, AC_ATTITUDE_CONTROL_ACCEL_RP_MAX_DEFAULT_CDSS),

    // @Param: XL_P_MX
    // @DisplayName: Acceleration Max for Pitch
    // @Description: Maximum acceleration in pitch axis
    // @Units: cdeg/s/s
    // @Range: 0 180000
    // @Increment: 1000
    // @Values: 0:Disabled, 30000:VerySlow, 72000:Slow, 108000:Medium, 162000:Fast
    // @User: Advanced
    AP_GROUPINFO("XL_P_MX", 12, ToyMode::Profile, accel_pitch_max, AC_ATTITUDE_CONTROL_ACCEL_RP_MAX_DEFAULT_CDSS),

    // @Param: PLT_V_UP
    // @DisplayName: Pilot maximum vertical speed ascending
    // @Description: The maximum vertical ascending velocity the pilot may request in cm/s
    // @Units: cm/s
    // @Range: 50 500
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("PL_V_UP", 13, ToyMode::Profile, pilot_speed_up, PILOT_VELZ_MAX),

    // @Param: FEEL_RP
    // @DisplayName: RC Feel Roll/Pitch
    // @Description: RC feel for roll/pitch which controls vehicle response to user input with 0 being extremely soft and 100 being crisp
    // @Range: 0 100
    // @Increment: 10
    // @User: Standard
    // @Values: 0:Very Soft, 25:Soft, 50:Medium, 75:Crisp, 100:Very Crisp
    AP_GROUPINFO("FEEL_RP", 14, ToyMode::Profile, rc_feel_rp,  RC_FEEL_RP_MEDIUM),

    // @Param: WP_SPD
    // @DisplayName: Waypoint Horizontal Speed Target
    // @Description: Defines the speed in cm/s which the aircraft will attempt to maintain horizontally during a WP mission
    // @Units: cm/s
    // @Range: 20 2000
    // @Increment: 50
    // @User: Standard
    AP_GROUPINFO("WP_SPD", 15, ToyMode::Profile, wp_speed_cms, WPNAV_WP_SPEED),

    // @Param: WP_RAD
    // @DisplayName: Waypoint Radius
    // @Description: Defines the distance from a waypoint, that when crossed indicates the wp has been hit.
    // @Units: cm
    // @Range: 10 1000
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("WP_RAD", 16, ToyMode::Profile, wp_radius_cm, WPNAV_WP_RADIUS),

    // @Param: WP_V_UP
    // @DisplayName: Waypoint Climb Speed Target
    // @Description: Defines the speed in cm/s which the aircraft will attempt to maintain while climbing during a WP mission
    // @Units: cm/s
    // @Range: 10 1000
    // @Increment: 50
    // @User: Standard
    AP_GROUPINFO("WP_V_UP", 17, ToyMode::Profile, wp_speed_up_cms, WPNAV_WP_SPEED_UP),

    // @Param: WP_V_DN
    // @DisplayName: Waypoint Descent Speed Target
    // @Description: Defines the speed in cm/s which the aircraft will attempt to maintain while descending during a WP mission
    // @Units: cm/s
    // @Range: 10 500
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("WP_V_DN", 18, ToyMode::Profile, wp_speed_down_cms, WPNAV_WP_SPEED_DOWN),

    // @Param: LTR_SPD
    // @DisplayName: Loiter Horizontal Maximum Speed
    // @Description: Defines the maximum speed in cm/s which the aircraft will travel horizontally while in loiter mode
    // @Units: cm/s
    // @Range: 20 2000
    // @Increment: 50
    // @User: Standard
    AP_GROUPINFO("LTR_SPD", 19, ToyMode::Profile, loiter_speed_cms, WPNAV_LOITER_SPEED),

    // @Param: WP_XL
    // @DisplayName: Waypoint Acceleration 
    // @Description: Defines the horizontal acceleration in cm/s/s used during missions
    // @Units: cm/s/s
    // @Range: 50 500
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("WP_XL", 20, ToyMode::Profile, wp_accel_cms, WPNAV_ACCELERATION),

    // @Param: WP_XL_Z
    // @DisplayName: Waypoint Vertical Acceleration
    // @Description: Defines the vertical acceleration in cm/s/s used during missions
    // @Units: cm/s/s
    // @Range: 50 500
    // @Increment: 10
    // @User: Standard
    AP_GROUPINFO("WP_XL_Z", 21, ToyMode::Profile, wp_accel_z_cms, WPNAV_WP_ACCEL_Z_DEFAULT),

    // @Param: LTR_JRK
    // @DisplayName: Loiter maximum jerk
    // @Description: Loiter maximum jerk in cm/s/s/s
    // @Units: cm/s/s/s
    // @Range: 500 5000
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("LTR_JRK", 22, ToyMode::Profile, loiter_jerk_max_cmsss, WPNAV_LOITER_JERK_MAX_DEFAULT),

    // @Param: LTR_MXA
    // @DisplayName: Loiter maximum acceleration
    // @Description: Loiter maximum acceleration in cm/s/s.  Higher values cause the copter to accelerate and stop more quickly.
    // @Units: cm/s/s
    // @Range: 100 981
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("LTR_MXA", 23, ToyMode::Profile, loiter_accel_cmss, WPNAV_LOITER_ACCEL),

    // @Param: LTR_MNA
    // @DisplayName: Loiter minimum acceleration
    // @Description: Loiter minimum acceleration in cm/s/s. Higher values stop the copter more quickly when the stick is centered, but cause a larger jerk when the copter stops.
    // @Units: cm/s/s
    // @Range: 25 250
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("LTR_MNA", 24, ToyMode::Profile, loiter_accel_min_cmss, WPNAV_LOITER_ACCEL_MIN),

    AP_GROUPEND

};

ToyMode::ToyMode()
{
    AP_Param::setup_object_defaults(this, var_info);
}

/*
  special mode handling for toys
 */
void ToyMode::update()
{
    if (!enable) {
        // not enabled
        return;
    }

    param_update();

#if ENABLE_LOAD_TEST
    if (!copter.motors->armed()) {
        load_test.running = false;
    }
#endif

    // keep filtered battery voltage for thrust limiting
    filtered_voltage = 0.99 * filtered_voltage + 0.01 * copter.battery.voltage();
    
    // update LEDs
    blink_update();
    
    if (!done_first_update) {
        done_first_update = true;

        /*
          fixup default primary mode. A frame with a flow sensor should default to FLOWHOLD as primary mode.
         */
        if (copter.optflow.device_id() != 0) {
            if (!primary_mode[0].configured_in_storage()) {
                primary_mode[0].set(FLOWHOLD);
            }
        }
        
        copter.set_mode(control_mode_t(primary_mode[0].get()), MODE_REASON_TMODE);
        copter.motors->set_thrust_compensation_callback(FUNCTOR_BIND_MEMBER(&ToyMode::thrust_limiting, void, float *, uint8_t));
    }

    // check if we should auto-trim
    if (trim_auto > 0) {
        trim_update();
    }
            
    // set ALT_HOLD as indoors for the EKF (disables GPS vertical velocity fusion)
#if 0
    copter.ahrs.set_indoor_mode(copter.control_mode == ALT_HOLD || copter.control_mode == FLOWHOLD);
#endif
    
    bool mode_button = false;
    bool right_button = false;
    bool left_action_button = false;
    bool right_action_button = false;
    bool power_button = false;
    bool mode_button_change = false;
    
    uint16_t ch5_in = hal.rcin->read(CH_5);
    uint16_t ch6_in = hal.rcin->read(CH_6);
    uint16_t ch7_in = hal.rcin->read(CH_7);

    if (copter.failsafe.radio || ch5_in < 900) {
        // failsafe handling is outside the scope of toy mode, it does
        // normal failsafe actions, just setup a blink pattern
        green_blink_pattern = BLINK_NO_RX;
        red_blink_pattern = BLINK_NO_RX;
        red_blink_index = green_blink_index;
        return;
    }

    uint32_t now = AP_HAL::millis();
    
    if (is_v2450_buttons()) {
        // V2450 button mapping from cypress radio. It maps the
        // buttons onto channels 5, 6 and 7 in a complex way, with the
        // left button latching
        mode_button_change = ((ch5_in > 1700 && last_ch5 <= 1700) || (ch5_in <= 1700 && last_ch5 > 1700));
        
        last_ch5 = ch5_in;
                        
        // get buttons from channels
        mode_button = (ch5_in > 2050 || (ch5_in > 1050 && ch5_in < 1150));
        right_button = (ch6_in > 1500);
        uint8_t ch7_bits = (ch7_in>1000)?uint8_t((ch7_in-1000)/100):0;
        left_action_button = (ch7_bits&1) != 0;
        right_action_button = (ch7_bits&2) != 0;
        power_button = (ch7_bits&4) != 0;
    } else if (is_f412_buttons()) {
        // F412 button setup for cc2500 radio. This maps the 6 buttons
        // onto channels 5 and 6, with no latching
        uint8_t ch5_bits = (ch5_in>1000)?uint8_t((ch5_in-1000)/100):0;
        uint8_t ch6_bits = (ch6_in>1000)?uint8_t((ch6_in-1000)/100):0;
        mode_button = (ch5_bits & 1) != 0;
        right_button = (ch5_bits & 2) != 0;
        right_action_button = (ch6_bits & 1) != 0;
        left_action_button = (ch6_bits & 2) != 0;
        power_button = (ch6_bits & 4) != 0;
        mode_button_change = (mode_button != last_mode_button);
        last_mode_button = mode_button;
    }
    
    // decode action buttons into an action
    uint8_t action_input = 0;    
    if (left_action_button) {
        action_input = 1;
    } else if (right_action_button) {
        action_input = 2;
    } else if (power_button) {
        action_input = 3;
    }
    
    if (action_input != 0 && mode_button) {
        // combined button actions
        action_input += 3;
        mode_press_counter = 0;
    } else if (mode_button) {
        mode_press_counter++;
    } else {
        mode_press_counter = 0;
    }

    bool reset_combination = left_action_button && right_action_button;
    if (reset_combination && abs(copter.ahrs.roll_sensor) > 160) {
        /*
          if both shoulder buttons are pressed at the same time for 5
          seconds while the vehicle is inverted then we send a
          WIFIRESET message to the sonix to reset SSID and password
        */
        if (reset_turtle_start_ms == 0) {
            reset_turtle_start_ms = now;
        }
        if (now - reset_turtle_start_ms > TOY_RESET_TURTLE_TIME) {
            gcs().send_text(MAV_SEVERITY_INFO, "Tmode: WiFi reset");
            reset_turtle_start_ms = 0;
            send_named_int("WIFIRESET", 1);
        }
    } else {
        reset_turtle_start_ms = 0;
    }
    if (reset_combination) {
        // don't act on buttons when combination pressed
        action_input = 0;
        mode_press_counter = 0;
    }

    /*
      work out commanded action, if any
     */
    enum toy_action action = action_input?toy_action(actions[action_input-1].get()):ACTION_NONE;
   
    // check for long left button press
    if (action == ACTION_NONE && mode_press_counter > TOY_LONG_PRESS_COUNT) {
        mode_press_counter = -TOY_COMMAND_DELAY;
        action = toy_action(actions[7].get());
        ignore_mode_button_change = true;
    }

    // cope with long left press triggering a left change
    if (ignore_mode_button_change && mode_button_change) {
        mode_button_change = false;
        ignore_mode_button_change = false;
    }

    if (is_v2450_buttons()) {
        // check for left button latching change
        if (action == ACTION_NONE && mode_button_change) {
            action = toy_action(actions[6].get());
        }
    } else if (is_f412_buttons()) {
        if (action == ACTION_NONE && mode_button_change && !mode_button) {
            // left release
            action = toy_action(actions[6].get());
        }

    }

    // check for right button
    if (action == ACTION_NONE && right_button) {
        right_press_counter++;
        if (right_press_counter >= TOY_RIGHT_PRESS_COUNT) {
            action = toy_action(actions[8].get());
            right_press_counter = -TOY_COMMAND_DELAY;
        }
    } else {
        right_press_counter = 0;
    }

    /*
      some actions shouldn't repeat too fast
     */
    switch (action) {
    case ACTION_TOGGLE_VIDEO:
    case ACTION_TOGGLE_MODE:
    case ACTION_TOGGLE_SIMPLE:
    case ACTION_TOGGLE_SSIMPLE:
    case ACTION_ARM_LAND_RTL:
    case ACTION_LOAD_TEST:
    case ACTION_MODE_FLOW:
        if (last_action == action ||
            now - last_action_ms < TOY_ACTION_DELAY_MS) {
            // for the above actions, button must be released before
            // it will activate again
            last_action = action;
            action = ACTION_NONE;
        }
        break;
        
    case ACTION_TAKE_PHOTO:
        // allow photo continuous shooting
        if (now - last_action_ms < TOY_ACTION_DELAY_MS) {
            last_action = action;
            action = ACTION_NONE;
        }
        break;

    default:
        last_action = action;
        break;
    }
    
    if (action != ACTION_NONE) {
        gcs().send_text(MAV_SEVERITY_INFO, "Tmode: action %u", action);
        last_action_ms = now;
    }

    // we use 150 for throttle_at_min to cope with varying stick throws
    bool throttle_at_min =
        copter.channel_throttle->get_control_in() < 150;

    // throttle threshold for throttle arming
    bool throttle_near_max =
        copter.channel_throttle->get_control_in() > 700;
    
    /*
      disarm if throttle is low for 1 second when landed
     */
    if ((flags & FLAG_THR_DISARM) && throttle_at_min && copter.motors->armed() && copter.ap.land_complete) {
        throttle_low_counter++;
        const uint8_t disarm_limit = copter.flightmode->has_manual_throttle()?TOY_LAND_MANUAL_DISARM_COUNT:TOY_LAND_DISARM_COUNT;
        if (throttle_low_counter >= disarm_limit) {
            gcs().send_text(MAV_SEVERITY_INFO, "Tmode: throttle disarm");
            copter.init_disarm_motors();
        }
    } else {
        throttle_low_counter = 0;
    }

    /*
      arm if throttle is high for 1 second when landed
     */
    if ((flags & FLAG_THR_ARM) && throttle_near_max && !copter.motors->armed()) {
        throttle_high_counter++;
        if (throttle_high_counter >= TOY_LAND_ARM_COUNT) {
            gcs().send_text(MAV_SEVERITY_INFO, "Tmode: throttle arm");
            arm_check_compass();
            if (!copter.init_arm_motors(true) && (flags & FLAG_UPGRADE_LOITER) && copter.control_mode == LOITER) {
                /*
                  support auto-switching to ALT_HOLD, then upgrade to LOITER once GPS available
                 */
                if (set_and_remember_mode(ALT_HOLD, MODE_REASON_TMODE)) {
                    gcs().send_text(MAV_SEVERITY_INFO, "Tmode: ALT_HOLD update arm");
#if AC_FENCE == ENABLED
                    copter.fence.enable(false);
#endif
                    if (!copter.init_arm_motors(true)) {
                        // go back to LOITER
                        gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: ALT_HOLD arm failed");
                        set_and_remember_mode(LOITER, MODE_REASON_TMODE);
                    } else {
                        upgrade_to_loiter = true;
#if 0
                        AP_Notify::flags.hybrid_loiter = true;
#endif
                    }
                }
            } else {
                throttle_arm_ms = AP_HAL::millis();
            }
        }
    } else {
        throttle_high_counter = 0;
    }

    if (upgrade_to_loiter) {
        if (!copter.motors->armed() || copter.control_mode != ALT_HOLD) {
            upgrade_to_loiter = false;
#if 0
            AP_Notify::flags.hybrid_loiter = false;
#endif
        } else if (copter.position_ok() && set_and_remember_mode(LOITER, MODE_REASON_TMODE)) {
#if AC_FENCE == ENABLED
            copter.fence.enable(true);
#endif
            gcs().send_text(MAV_SEVERITY_INFO, "Tmode: LOITER update");            
        }
    }

    if (copter.control_mode == RTL && (flags & FLAG_RTL_CANCEL) && throttle_near_max) {
        gcs().send_text(MAV_SEVERITY_INFO, "Tmode: RTL cancel");        
        set_and_remember_mode(LOITER, MODE_REASON_TMODE);
    }
    
    enum control_mode_t old_mode = copter.control_mode;
    enum control_mode_t new_mode = old_mode;

    /*
      implement actions
     */
    switch (action) {
    case ACTION_NONE:
        break;

    case ACTION_TAKE_PHOTO:
        send_named_int("SNAPSHOT", 1);
        break;

    case ACTION_TOGGLE_VIDEO:
        send_named_int("VIDEOTOG", 1);
        break;

    case ACTION_MODE_ACRO:
        new_mode = ACRO;
        break;
        
    case ACTION_MODE_ALTHOLD:
        new_mode = ALT_HOLD;
        break;

    case ACTION_MODE_AUTO:
        new_mode = AUTO;
        break;

    case ACTION_MODE_LOITER:
        new_mode = LOITER;
        break;

    case ACTION_MODE_RTL:
        new_mode = RTL;
        break;

    case ACTION_MODE_CIRCLE:
        new_mode = CIRCLE;
        break;

    case ACTION_MODE_LAND:
        new_mode = LAND;
        break;

    case ACTION_MODE_DRIFT:
        new_mode = DRIFT;
        break;

    case ACTION_MODE_SPORT:
        new_mode = SPORT;
        break;

    case ACTION_MODE_AUTOTUNE:
        new_mode = AUTOTUNE;
        break;

    case ACTION_MODE_POSHOLD:
        new_mode = POSHOLD;
        break;

    case ACTION_MODE_BRAKE:
        new_mode = BRAKE;
        break;

    case ACTION_MODE_THROW:
        new_mode = THROW;
        break;

    case ACTION_MODE_FLIP:
        new_mode = FLIP;
        break;

    case ACTION_MODE_STAB:
        new_mode = STABILIZE;
        break;

    case ACTION_MODE_FLOW:
        // toggle flow hold
        if (old_mode != FLOWHOLD) {
            new_mode = FLOWHOLD;
        } else {
            new_mode = ALT_HOLD;
        }
        break;
        
    case ACTION_DISARM:
        if (copter.motors->armed()) {
            gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: Force disarm");
            copter.init_disarm_motors();
        }
        break;

    case ACTION_TOGGLE_MODE:
        last_mode_choice = (last_mode_choice+1) % 2;
        new_mode = control_mode_t(primary_mode[last_mode_choice].get());
        break;

    case ACTION_TOGGLE_SIMPLE:
        copter.set_simple_mode(copter.ap.simple_mode?0:1);
        break;

    case ACTION_TOGGLE_SSIMPLE:
        copter.set_simple_mode(copter.ap.simple_mode?0:2);
        break;
        
    case ACTION_ARM_LAND_RTL:
        if (!copter.motors->armed()) {
            action_arm();
        } else if (old_mode == RTL) {
            // switch between RTL and LOITER when in GPS modes
            new_mode = LOITER;
        } else if (old_mode == LAND) {
            if (last_set_mode == LAND || !copter.position_ok()) {
                // this is a land that we asked for, or we don't have good positioning
                new_mode = ALT_HOLD;
            } else if (copter.landing_with_GPS()) {
                new_mode = LOITER;
            } else {
                new_mode = ALT_HOLD;
            }
        } else if (copter.flightmode->requires_GPS()) {
            // if we're in a GPS mode, then RTL
            new_mode = RTL;
        } else {
            // if we're in a non-GPS mode, then LAND
            new_mode = LAND;
        }
        break;

    case ACTION_LOAD_TEST:
#if ENABLE_LOAD_TEST
        if (copter.motors->armed() && !load_test.running) {
            break;
        }
        if (load_test.running) {
            load_test.running = false;
            gcs().send_text(MAV_SEVERITY_INFO, "Tmode: load_test off");
            copter.init_disarm_motors();
            copter.set_mode(ALT_HOLD, MODE_REASON_TMODE);
        } else {
            copter.set_mode(ALT_HOLD, MODE_REASON_TMODE);
#if AC_FENCE == ENABLED
            copter.fence.enable(false);
#endif
            if (copter.init_arm_motors(true)) {
                load_test.running = true;
                gcs().send_text(MAV_SEVERITY_INFO, "Tmode: load_test on");
            } else {
                gcs().send_text(MAV_SEVERITY_INFO, "Tmode: load_test failed");
            }
        }
#endif
        break;
    }

    if (!copter.motors->armed() && (copter.control_mode == LAND || copter.control_mode == RTL)) {
        // revert back to last primary flight mode if disarmed after landing
        new_mode = control_mode_t(primary_mode[last_mode_choice].get());
    }
    
    if (new_mode != copter.control_mode) {
        load_test.running = false;
#if AC_FENCE == ENABLED
        copter.fence.enable(false);
#endif
        if (set_and_remember_mode(new_mode, MODE_REASON_TX_COMMAND)) {
            gcs().send_text(MAV_SEVERITY_INFO, "Tmode: mode %s", copter.flightmode->name4());
            // force fence on in all GPS flight modes
#if AC_FENCE == ENABLED
            if (copter.flightmode->requires_GPS()) {
                copter.fence.enable(true);
            }
#endif
        } else {
            gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: %u FAILED", (unsigned)new_mode);
            if (new_mode == RTL) {
                // if we can't RTL then land
                gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: LANDING");
                set_and_remember_mode(LAND, MODE_REASON_TMODE);
#if AC_FENCE == ENABLED
                if (copter.landing_with_GPS()) {
                    copter.fence.enable(true);
                }
#endif
            }
        }
    }
}


void ToyMode::param_update()
{
    if (!ptr_to_param_loaded) {
        enum ap_var_type dummy;
        Profile::param_ptr_to_acro_rp_p = AP_Param::find("ACRO_RP_P", &dummy);
        if (Profile::param_ptr_to_acro_rp_p == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_RP_P");
            return;
        }

        Profile::param_ptr_to_acro_yaw_p = AP_Param::find("ACRO_YAW_P", &dummy);
        if (Profile::param_ptr_to_acro_yaw_p == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_YAW_P");
            return;
        }

        Profile::param_ptr_to_acro_balance_roll = AP_Param::find("ACRO_BAL_ROLL", &dummy);
        if (Profile::param_ptr_to_acro_balance_roll == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_BAL_ROLL");
            return;
        }

        Profile::param_ptr_to_acro_balance_pitch = AP_Param::find("ACRO_BAL_PITCH", &dummy);
        if (Profile::param_ptr_to_acro_balance_pitch == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_BAL_PITCH");
            return;
        }

        Profile::param_ptr_to_acro_trainer = AP_Param::find("ACRO_TRAINER", &dummy);
        if (Profile::param_ptr_to_acro_trainer == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_TRAINER");
            return;
        }
       
        Profile::param_ptr_to_acro_rp_expo = AP_Param::find("ACRO_RP_EXPO", &dummy);
        if (Profile::param_ptr_to_acro_rp_expo == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_RP_EXPO");
            return;
        }

        Profile::param_ptr_to_acro_y_expo = AP_Param::find("ACRO_Y_EXPO", &dummy);
        if (Profile::param_ptr_to_acro_y_expo == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_Y_EXPO");
            return;
        }

        Profile::param_ptr_to_acro_thr_mid = AP_Param::find("ACRO_THR_MID", &dummy);
        if (Profile::param_ptr_to_acro_thr_mid == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ACRO_THR_MID");
            return;
        }

        Profile::param_ptr_to_angle_max = AP_Param::find("ANGLE_MAX", &dummy);
        if (Profile::param_ptr_to_angle_max == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ANGLE_MAX");
            return;
        }

        Profile::param_ptr_to_accel_roll_max = AP_Param::find("ATC_ACCEL_R_MAX", &dummy);
        if (Profile::param_ptr_to_accel_roll_max == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ATC_ACCEL_R_MAX");
            return;
        }

        Profile::param_ptr_to_accel_pitch_max = AP_Param::find("ATC_ACCEL_P_MAX", &dummy);
        if (Profile::param_ptr_to_accel_pitch_max == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ATC_ACCEL_P_MAX");
            return;
        }

        Profile::param_ptr_to_accel_yaw_max = AP_Param::find("ATC_ACCEL_Y_MAX", &dummy);
        if (Profile::param_ptr_to_accel_yaw_max == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "ATC_ACCEL_Y_MAX");
            return;
        }

        Profile::param_ptr_to_pilot_speed_up = AP_Param::find("PILOT_SPEED_UP", &dummy);
        if (Profile::param_ptr_to_pilot_speed_up == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "PILOT_SPEED_UP");
            return;
        }

        Profile::param_ptr_to_rc_feel_rp = AP_Param::find("RC_FEEL_RP", &dummy);
        if (Profile::param_ptr_to_rc_feel_rp == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "RC_FEEL_RP");
            return;
        }

        Profile::param_ptr_to_loiter_speed_cms = AP_Param::find("WPNAV_LOIT_SPEED", &dummy);
        if (Profile::param_ptr_to_loiter_speed_cms == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_LOIT_SPEED");
            return;
        }

        Profile::param_ptr_to_loiter_jerk_max_cmsss = AP_Param::find("WPNAV_LOIT_JERK", &dummy);
        if (Profile::param_ptr_to_loiter_jerk_max_cmsss == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_LOIT_JERK");
            return;
        }
        
        Profile::param_ptr_to_loiter_accel_cmss = AP_Param::find("WPNAV_LOIT_MAXA", &dummy);
        if (Profile::param_ptr_to_loiter_accel_cmss == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_LOIT_MAXA");
            return;
        }

        Profile::param_ptr_to_loiter_accel_min_cmss = AP_Param::find("WPNAV_LOIT_MINA", &dummy);
        if (Profile::param_ptr_to_loiter_accel_min_cmss == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_LOIT_MINA");
            return;
        }
        
        Profile::param_ptr_to_wp_speed_cms = AP_Param::find("WPNAV_SPEED", &dummy);
        if (Profile::param_ptr_to_wp_speed_cms == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_SPEED");
            return;
        }

        Profile::param_ptr_to_wp_speed_up_cms = AP_Param::find("WPNAV_SPEED_UP", &dummy);
        if (Profile::param_ptr_to_wp_speed_up_cms == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_SPEED_UP");
            return;
        }

        Profile::param_ptr_to_wp_speed_down_cms = AP_Param::find("WPNAV_SPEED_DN", &dummy);
        if (Profile::param_ptr_to_wp_speed_down_cms == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_SPEED_DN");
            return;
        }

        Profile::param_ptr_to_wp_radius_cm = AP_Param::find("WPNAV_RADIUS", &dummy);
        if (Profile::param_ptr_to_wp_radius_cm == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_RADIUS");
            return;
        }

        Profile::param_ptr_to_wp_accel_cms = AP_Param::find("WPNAV_ACCEL", &dummy);
        if (Profile::param_ptr_to_wp_accel_cms == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_ACCEL");
            return;
        }

        Profile::param_ptr_to_wp_accel_z_cms = AP_Param::find("WPNAV_ACCEL_Z", &dummy);
        if (Profile::param_ptr_to_wp_accel_z_cms == nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Profile unable to find %s\n", "WPNAV_ACCEL_Z");
            return;
        }
        ptr_to_param_loaded = true;
    } else if(profile_id != 0){
        Profile::param_ptr_to_acro_rp_p->set_float(_var_info_profile[profile_id - 1].acro_rp_p.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_acro_yaw_p->set_float(_var_info_profile[profile_id - 1].acro_yaw_p.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_acro_balance_roll->set_float(_var_info_profile[profile_id - 1].acro_balance_roll.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_acro_balance_pitch->set_float(_var_info_profile[profile_id - 1].acro_balance_pitch.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_acro_trainer->set_float(_var_info_profile[profile_id - 1].acro_trainer.cast_to_float(), AP_PARAM_INT8);
        Profile::param_ptr_to_acro_rp_expo->set_float(_var_info_profile[profile_id - 1].acro_rp_expo.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_acro_y_expo->set_float(_var_info_profile[profile_id - 1].acro_y_expo.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_acro_thr_mid->set_float(_var_info_profile[profile_id - 1].acro_thr_mid.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_angle_max->set_float(_var_info_profile[profile_id - 1].angle_max.cast_to_float(), AP_PARAM_INT16);
        Profile::param_ptr_to_accel_roll_max->set_float(_var_info_profile[profile_id - 1].accel_roll_max.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_accel_pitch_max->set_float(_var_info_profile[profile_id - 1].accel_pitch_max.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_accel_yaw_max->set_float(_var_info_profile[profile_id - 1].accel_yaw_max.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_pilot_speed_up->set_float(_var_info_profile[profile_id - 1].pilot_speed_up.cast_to_float(), AP_PARAM_INT16);
        Profile::param_ptr_to_rc_feel_rp->set_float(_var_info_profile[profile_id - 1].rc_feel_rp.cast_to_float(), AP_PARAM_INT8);
        Profile::param_ptr_to_loiter_speed_cms->set_float(_var_info_profile[profile_id - 1].loiter_speed_cms.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_loiter_jerk_max_cmsss->set_float(_var_info_profile[profile_id - 1].loiter_jerk_max_cmsss.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_loiter_accel_cmss->set_float(_var_info_profile[profile_id - 1].loiter_accel_cmss.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_loiter_accel_min_cmss->set_float(_var_info_profile[profile_id - 1].loiter_accel_min_cmss.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_wp_speed_cms->set_float(_var_info_profile[profile_id - 1].wp_speed_cms.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_wp_speed_up_cms->set_float(_var_info_profile[profile_id - 1].wp_speed_up_cms.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_wp_speed_down_cms->set_float(_var_info_profile[profile_id - 1].wp_speed_down_cms.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_wp_radius_cm->set_float(_var_info_profile[profile_id - 1].wp_radius_cm.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_wp_accel_cms->set_float(_var_info_profile[profile_id - 1].wp_accel_cms.cast_to_float(), AP_PARAM_FLOAT);
        Profile::param_ptr_to_wp_accel_z_cms->set_float(_var_info_profile[profile_id - 1].wp_accel_z_cms.cast_to_float(), AP_PARAM_FLOAT);
        param_not_set_to_orig = false;
    } else if(param_not_set_to_orig){
        Profile::param_ptr_to_acro_rp_p->load();
        Profile::param_ptr_to_acro_yaw_p->load();
        Profile::param_ptr_to_acro_balance_roll->load();
        Profile::param_ptr_to_acro_balance_pitch->load();
        Profile::param_ptr_to_acro_trainer->load();
        Profile::param_ptr_to_acro_rp_expo->load();
        Profile::param_ptr_to_acro_y_expo->load();
        Profile::param_ptr_to_acro_thr_mid->load();
        Profile::param_ptr_to_angle_max->load();
        Profile::param_ptr_to_accel_roll_max->load();
        Profile::param_ptr_to_accel_pitch_max->load();
        Profile::param_ptr_to_accel_yaw_max->load();
        Profile::param_ptr_to_pilot_speed_up->load();
        Profile::param_ptr_to_rc_feel_rp->load();
        Profile::param_ptr_to_loiter_speed_cms->load();
        Profile::param_ptr_to_loiter_jerk_max_cmsss->load();
        Profile::param_ptr_to_loiter_accel_cmss->load();
        Profile::param_ptr_to_loiter_accel_min_cmss->load();
        Profile::param_ptr_to_wp_speed_cms->load();
        Profile::param_ptr_to_wp_speed_up_cms->load();
        Profile::param_ptr_to_wp_speed_down_cms->load();
        Profile::param_ptr_to_wp_radius_cm->load();
        Profile::param_ptr_to_wp_accel_cms->load();
        Profile::param_ptr_to_wp_accel_z_cms->load();
        param_not_set_to_orig = true;
    }
}


/*
  set a mode, remembering what mode we set, and the previous mode we were in
 */
bool ToyMode::set_and_remember_mode(control_mode_t mode, mode_reason_t reason)
{
    if (copter.control_mode == mode) {
        return true;
    }
    if (!copter.set_mode(mode, reason)) {
        return false;
    }
    last_set_mode = mode;
    return true;
}

/*
  automatic stick trimming. This works while disarmed by looking for
  zero rc-input changes for 4 seconds, and assuming sticks are
  centered. Trim is saved
 */
void ToyMode::trim_update(void)
{
    if (hal.util->get_soft_armed() || copter.failsafe.radio) {
        // only when disarmed and with RC link
        trim.start_ms = 0;
        return;
    }

    // get throttle mid from channel trim
    uint16_t throttle_trim = copter.channel_throttle->get_radio_trim();
    if (abs(throttle_trim - 1500) <= trim_auto) {
        RC_Channel *ch = copter.channel_throttle;
        uint16_t ch_min = ch->get_radio_min();
        uint16_t ch_max = ch->get_radio_max();
        // remember the throttle midpoint
        int16_t new_value = 1000UL * (throttle_trim - ch_min) / (ch_max - ch_min);
        if (new_value != throttle_mid) {
            throttle_mid = new_value;
            gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: thr mid %d\n",
                                             throttle_mid);
        }
    }
    
    uint16_t chan[4];
    if (hal.rcin->read(chan, 4) != 4) {
        trim.start_ms = 0;
        return;
    }

    const uint16_t noise_limit = 2;
    for (uint8_t i=0; i<4; i++) {
        if (abs(chan[i] - 1500) > trim_auto) {
            // not within limit
            trim.start_ms = 0;
            return;
        }
    }

    uint32_t now = AP_HAL::millis();
    
    if (trim.start_ms == 0) {
        // start timer
        memcpy(trim.chan, chan, 4*sizeof(uint16_t));
        trim.start_ms = now;
        return;
    }

    
    for (uint8_t i=0; i<4; i++) {
        if (abs(trim.chan[i] - chan[i]) > noise_limit) {
            // detected stick movement
            memcpy(trim.chan, chan, 4*sizeof(uint16_t));
            trim.start_ms = now;
            return;
        }
    }

    if (now - trim.start_ms < 4000) {
        // not steady for long enough yet
        return;
    }

    // reset timer so we don't trigger too often
    trim.start_ms = 0;
    
    uint8_t need_trim = 0;
    for (uint8_t i=0; i<4; i++) {
        RC_Channel *ch = RC_Channels::rc_channel(i);
        if (ch && abs(chan[i] - ch->get_radio_trim()) > noise_limit) {
            need_trim |= 1U<<i;
        }
    }
    if (need_trim == 0) {
        return;
    }
    for (uint8_t i=0; i<4; i++) {
        if (need_trim & (1U<<i)) {
            RC_Channel *ch = RC_Channels::rc_channel(i);
            ch->set_and_save_radio_trim(chan[i]);
        }
    }

    gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: trim %u %u %u %u\n",
                                     chan[0], chan[1], chan[2], chan[3]);
}

/*
  handle arming action
 */
void ToyMode::action_arm(void)
{
    bool needs_gps = copter.flightmode->requires_GPS();

    // don't arm if sticks aren't in deadzone, to prevent pot problems
    // on TX causing flight control issues
    bool sticks_centered =
        copter.channel_roll->get_control_in() == 0 &&
        copter.channel_pitch->get_control_in() == 0 &&
        copter.channel_yaw->get_control_in() == 0;

    if (!sticks_centered) {
        gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: sticks not centered\n");
        return;
    }

    arm_check_compass();
    
    if (needs_gps && copter.arming.gps_checks(false)) {
#if AC_FENCE == ENABLED
        // we want GPS and checks are passing, arm and enable fence
        copter.fence.enable(true);
#endif
        copter.init_arm_motors(false);
        if (!copter.motors->armed()) {
            AP_Notify::events.arming_failed = true;
            gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: GPS arming failed");
        } else {
            gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: GPS armed motors");
        }
    } else if (needs_gps) {
        // notify of arming fail
        AP_Notify::events.arming_failed = true;
        gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: GPS arming failed");
    } else {
#if AC_FENCE == ENABLED
        // non-GPS mode
        copter.fence.enable(false);
#endif
        copter.init_arm_motors(false);
        if (!copter.motors->armed()) {
            AP_Notify::events.arming_failed = true;
            gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: non-GPS arming failed");
        } else {
            gcs().send_text(MAV_SEVERITY_ERROR, "Tmode: non-GPS armed motors");
        }
    }
}

/*
  adjust throttle for throttle takeoff
  This prevents sudden climbs when using throttle for arming
*/
void ToyMode::throttle_adjust(float &throttle_control)
{
    uint32_t now = AP_HAL::millis();
    const uint32_t soft_start_ms = 5000;
    const uint16_t throttle_start = 600 + copter.g.throttle_deadzone;
    if (!copter.motors->armed() && (flags & FLAG_THR_ARM)) {
        throttle_control = MIN(throttle_control, 500);
    } else if (now - throttle_arm_ms < soft_start_ms) {
        float p = (now - throttle_arm_ms) / float(soft_start_ms);
        throttle_control = MIN(throttle_control, throttle_start + p * (1000 - throttle_start));
    }

    // limit descent rate close to the ground
    float height = copter.inertial_nav.get_altitude() * 0.01 - copter.arming_altitude_m;
    if (throttle_control < 500 &&
        height < TOY_DESCENT_SLOW_HEIGHT + TOY_DESCENT_SLOW_RAMP &&
        copter.motors->armed() && !copter.ap.land_complete) {
        float limit = linear_interpolate(TOY_DESCENT_SLOW_MIN, 0, height,
                                         TOY_DESCENT_SLOW_HEIGHT, TOY_DESCENT_SLOW_HEIGHT+TOY_DESCENT_SLOW_RAMP);
        if (throttle_control < limit) {
            // limit descent rate close to the ground
            throttle_control = limit;
        }
    }
}


/*
  update blinking. Blinking is done with a 16 bit pattern for each
  LED. A count can be set for a pattern, which makes the pattern
  persist until the count is zero. When it is zero the normal pattern
  settings based on system status are used
 */
void ToyMode::blink_update(void)
{
    if (red_blink_pattern & (1U<<red_blink_index)) {
        copter.relay.on(1);
    } else {
        copter.relay.off(1);
    }
    if (green_blink_pattern & (1U<<green_blink_index)) {
        copter.relay.on(0);
    } else {
        copter.relay.off(0);
    }
    green_blink_index = (green_blink_index+1) % 16;
    red_blink_index = (red_blink_index+1) % 16;
    if (green_blink_index == 0 && green_blink_count > 0) {
        green_blink_count--;
    }
    if (red_blink_index == 0 && red_blink_count > 0) {
        red_blink_count--;
    }

    // let the TX know we are recording video
    uint32_t now = AP_HAL::millis();
    if (now - last_video_ms < 1000) {
        AP_Notify::flags.video_recording = 1;
    } else {
        AP_Notify::flags.video_recording = 0;
    }
    
    if (red_blink_count > 0 && green_blink_count > 0) {
        return;
    }
    
    // setup normal patterns based on flight mode and arming
    uint16_t pattern = 0;

    // full on when we can see the TX, except for battery failsafe,
    // when we blink rapidly
    if (copter.motors->armed() && AP_Notify::flags.failsafe_battery) {
        pattern = BLINK_8;
    } else if (!copter.motors->armed() && (blink_disarm > 0)) {
		pattern = BLINK_8;
		blink_disarm--;
	} else {
        pattern = BLINK_FULL;
    }
    
    if (copter.motors->armed()) {
		blink_disarm = 4;
	}
    
    if (red_blink_count == 0) {
        red_blink_pattern = pattern;
    }
    if (green_blink_count == 0) {
        green_blink_pattern = pattern;
    }
    if (red_blink_count == 0 && green_blink_count == 0) {
        // get LEDs in sync
        red_blink_index = green_blink_index;
    }
}

// handle a mavlink message
void ToyMode::handle_message(mavlink_message_t *msg)
{
    if (msg->msgid != MAVLINK_MSG_ID_NAMED_VALUE_INT) {
        return;
    }
    mavlink_named_value_int_t m;
    mavlink_msg_named_value_int_decode(msg, &m);
    if (strncmp(m.name, "BLINKR", 10) == 0) {
        red_blink_pattern = (uint16_t)m.value;
        red_blink_count = m.value >> 16;
        red_blink_index = 0;
    } else if (strncmp(m.name, "BLINKG", 10) == 0) {
        green_blink_pattern = (uint16_t)m.value;
        green_blink_count = m.value >> 16;
        green_blink_index = 0;
    } else if (strncmp(m.name, "VNOTIFY", 10) == 0) {
        // taking photos or video
        if (green_blink_pattern != BLINK_2) {
            green_blink_index = 0;
        }
        green_blink_pattern = BLINK_2;
        green_blink_count = 1;
        last_video_ms = AP_HAL::millis();
        // immediately update AP_Notify recording flag
        AP_Notify::flags.video_recording = 1;
    } else if (strncmp(m.name, "WIFICHAN", 10) == 0) {
#ifdef HAL_RCINPUT_WITH_AP_RADIO
        AP_Radio *radio = AP_Radio::instance();
        if (radio) {
            radio->set_wifi_channel(m.value);
        }
#endif
    } else if (strncmp(m.name, "LOGDISARM", 10) == 0) {
        enum ap_var_type vtype;
        AP_Int8 *log_disarmed = (AP_Int8 *)AP_Param::find("LOG_DISARMED", &vtype);
        if (log_disarmed) {
            log_disarmed->set(int8_t(m.value));
        }
    }
}

/*
  send a named int to primary telem channel
 */
void ToyMode::send_named_int(const char *name, int32_t value)
{
    mavlink_msg_named_value_int_send(MAVLINK_COMM_1, AP_HAL::millis(), name, value);
}

/*
  limit maximum thrust based on voltage
 */
void ToyMode::thrust_limiting(float *thrust, uint8_t num_motors)
{
    float thrust_mul = linear_interpolate(filter.thrust_max, filter.thrust_min, filtered_voltage, filter.volt_min, filter.volt_max);
    for (uint8_t i=0; i<num_motors; i++) {
        thrust[i] *= thrust_mul;
    }
    uint16_t pwm[4];
    hal.rcout->read(pwm, 4);
    if (motor_log_counter++ % 10 == 0) {
        DataFlash_Class::instance()->Log_Write("THST", "TimeUS,Vol,Mul,M1,M2,M3,M4", "QffHHHH",
                                               AP_HAL::micros64(),
                                               (double)filtered_voltage,
                                               (double)thrust_mul,
                                               pwm[0], pwm[1], pwm[2], pwm[3]);
    }
                                           
}

#if ENABLE_LOAD_TEST
/*
  run a motor load test - used for endurance checking in factory tests
 */
void ToyMode::load_test_run(void)
{
    uint16_t pwm[4] {};
    switch ((enum load_type)load_test.load_type.get()) {
    case LOAD_TYPE_LOG1:
        for (uint8_t i=0; i<4; i++) {
            pwm[i] = load_data1[load_test.row].m[i];
        }
        load_test.filter_counter++;
        if (load_test.filter_counter >= load_test.load_filter.get()) {
            load_test.filter_counter = 0;
            load_test.row = (load_test.row + 1) % ARRAY_SIZE(load_data1);
        }
        break;
        
    case LOAD_TYPE_LOG2:
        // like log1, but all the same
        for (uint8_t i=0; i<4; i++) {
            pwm[i] = load_data1[load_test.row].m[0];
        }
        load_test.filter_counter++;
        if (load_test.filter_counter >= load_test.load_filter.get()) {
            load_test.filter_counter = 0;
            load_test.row = (load_test.row + 1) % ARRAY_SIZE(load_data1);
        }
        break;

    case LOAD_TYPE_CONSTANT:
        for (uint8_t i=0; i<4; i++) {
            pwm[i] = 500;
        }
        break;
    default:
        return;
    }
    for (uint8_t i=0; i<4; i++) {
        pwm[i] *= load_test.load_mul;
        // write, with conversion to 1000 to 2000 range
        hal.rcout->write(i, 1000 + pwm[i]*2);
    }

    if (copter.failsafe.battery) {
        gcs().send_text(MAV_SEVERITY_INFO, "Tmode: load_test off (battery)");
        copter.init_disarm_motors();
        load_test.running = false;
    }    
}
#endif // ENABLE_LOAD_TEST

/*
  if we try to arm and the compass is out of range then we enable
  inflight compass learning
 */
void ToyMode::arm_check_compass(void)
{
    // check for unreasonable compass offsets
    Vector3f offsets = copter.compass.get_offsets();
    float field = copter.compass.get_field().length();
    
    if (offsets.length() > copter.compass.get_offsets_max() ||
        field < 200 || field > 800 ||
        !copter.compass.configured()) {
        if (copter.compass.get_learn_type() != Compass::LEARN_INFLIGHT) {
            gcs().send_text(MAV_SEVERITY_INFO, "Tmode: enable compass learning");
            copter.compass.set_learn_type(Compass::LEARN_INFLIGHT, false);
        }
    }
}

#endif // TOY_MODE_ENABLED
