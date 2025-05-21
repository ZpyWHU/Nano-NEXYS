#ifndef _KALMAN_FILTER_H_
#define _KALMAN_FILTER_H_

typedef struct
{
    float q; // 过程噪声
    float r; // 观测噪声
    float x; // 最佳估计值
    float p; // 估计误差协方差
    float k; // 卡尔曼增益
} KalmanFilter;

void kalman_init(KalmanFilter *kf, float process_noise, float measurement_noise, float estimated_error, float initial_value);
float kalman_update(KalmanFilter *kf, float measurement);

#endif
