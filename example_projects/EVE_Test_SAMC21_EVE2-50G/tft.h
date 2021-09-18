
#ifndef TFT_H
#define TFT_H

extern uint16_t num_profile_a, num_profile_b;

#ifdef __cplusplus
extern "C" {
#endif

void TFT_init(void);
void TFT_touch(void);
void TFT_display(void);

#ifdef __cplusplus
}
#endif

#endif /* TFT_H */
