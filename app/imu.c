#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "imu.h"
#include "accelgyro.h"
#include "magneto.h"
#include "mainloop_timer.h"

#define IMU_SAMPLE_FREQ       1000

////////////////////////////////////////////////////////////////////////////////
//
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static imu_t            _imu;
static SoftTimerElem    _sample_timer;
int16_t                 attitude[3];

int16_t                 accel_ned[3],
                        gyro_ned[3],
                        mag_ned[3];

////////////////////////////////////////////////////////////////////////////////
//
// sensor alignment
// both accel/gyro/mag
//
//  y
//  |
// <----
//  |
//  |       /\
//  |_______|_____x
//  z+      |
//  ZCCW    |
//
//
// madgwick coord
//
//  X (North, Roll)
//  |
//  |
//  |___________y (East, Pitch)
//  z+ (heading)
// 
// Pitch increases with north up
// Roll increases to the right bank
// Heading increases closewise.
//
// This is the required coordinate system
// and accel/mag coordinates and
// gyro directions should be adjusted
// according to the required coordinate system.
// 
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static void imu_ned_aling(imu_t* imu)
{
  accel_ned[0] = accel_value[1];
  accel_ned[1] = accel_value[0];
  accel_ned[2] = accel_value[2];

  gyro_ned[0]  = -gyro_dps[1];
  gyro_ned[1]  = -gyro_dps[0];
  gyro_ned[2]  = -gyro_dps[2];

  mag_ned[0] = mag_value[1];
  mag_ned[1] = mag_value[0];
  mag_ned[2] = mag_value[2];
}

static void
imu_run(imu_t* imu)
{
  imu_ned_aling(imu);

#if 0
  madgwick_update(&imu->filter,
      -gyro_dps[1],   -gyro_dps[0],   -gyro_dps[2],
      accel_value[1], accel_value[0], accel_value[2],
      mag_value[1],   mag_value[0],   mag_value[2]);
#else
  madgwick_update(&imu->filter,
      gyro_ned[0],    gyro_ned[1],    gyro_ned[2],
      accel_ned[0],   accel_ned[1],   accel_ned[2],
      mag_ned[0],     mag_ned[1],     mag_ned[2]);
#endif

  madgwick_get_roll_pitch_yaw(&imu->filter,
      attitude, 0.0f);
}

static void
imu_sample_timer_callback(SoftTimerElem* te)
{
  imu_run(&_imu);

  mainloop_timer_schedule(&_sample_timer, 1);
}

////////////////////////////////////////////////////////////////////////////////
//
// module publics
//
////////////////////////////////////////////////////////////////////////////////
void
imu_init(void)
{
  memset(&_imu, 0, sizeof(_imu));

  madgwick_init(&_imu.filter, IMU_SAMPLE_FREQ);

  soft_timer_init_elem(&_sample_timer);
  _sample_timer.cb = imu_sample_timer_callback;

  mainloop_timer_schedule(&_sample_timer, 1);
}

imu_t*
imu_get(void)
{
  return &_imu;
}