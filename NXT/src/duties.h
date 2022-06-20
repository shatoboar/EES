/**
 * @file duties.h
 * @author Luca Jungnickel
 * @brief 
 * @version 0.1
 * @date 2022-05-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef DUTIES_H
#define DUTIES_H

void dispense();
void moveToCamera();
int getCameraData();
int moveToBox(int numberBox, int currentPosition);
void moveOneUnit(int direction, int speed);
int dropItem();

#endif //DUTIES_H