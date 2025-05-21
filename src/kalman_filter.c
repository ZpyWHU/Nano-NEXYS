#include "kalman_filter.h"

void kalman_init(KalmanFilter *kf, float process_noise, float measurement_noise, float estimated_error, float initial_value)
{
    kf->q = process_noise;
    kf->r = measurement_noise;
    kf->p = estimated_error;
    kf->x = initial_value;
    kf->k = 0;
}

float kalman_update(KalmanFilter *kf, float measurement)
{
    // 预测
    kf->p = kf->p + kf->q;

    // 更新
    kf->k = kf->p / (kf->p + kf->r);
    kf->x = kf->x + kf->k * (measurement - kf->x);
    kf->p = (1 - kf->k) * kf->p;

    return kf->x;
}
