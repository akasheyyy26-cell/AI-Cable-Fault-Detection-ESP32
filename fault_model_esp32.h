#pragma once

#define N_FEATURES 3
#define N_CLASSES  4

// Feature order must match training: [BusV, Current_mA, NodeV]

static const float SCALER_MEAN[3] = { 10.92986202f, 1012.84313965f, 1.05772722f };
static const float SCALER_SCALE[3] = { 0.87347096f, 565.99920654f, 1.26700115f };
static const float LR_W[4][3] = {
  { -1.09534478f, -1.65716445f, -1.26549709f },
  { 0.01738565f, 0.03463206f, 2.93628454f },
  { 2.32710028f, -3.01203823f, -0.79477745f },
  { -1.24914122f, 4.63457060f, -0.87601000f }
};
static const float LR_B[4] = { 2.03408813f, 1.16962624f, -0.99460751f, -2.20910668f };

// Class index -> label mapping (same order as sklearn clf.classes_)
#define CLASS_0 "LEAKAGE"
#define CLASS_1 "NORMAL"
#define CLASS_2 "OPEN"
#define CLASS_3 "SHORT"

